#include "pnvm_types.h"
#include "pnvm_plugin.h"
#include <SPI.h>
#include <Ethernet.h>

/* Our "dynamic object memory" used by MEMBLOCK library as a storage for
 * dynamically allocated blocks. It's aligned to the multiples of 4 - which
 * is a requirement for block addresses prescribed by PNVMToken.
 */
uint8_t block_heap[
    MEMBLOCK_BAGSIZE(AllocationBlockSize, TotalAllocationBlocks)]
    __attribute__ ((aligned (4)));
/* And this is a handler for MEMBLOCK library for this heap. */
MEMBLOCK_bag_t block_workspace;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDNS(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server(23);
boolean alreadyConnected = false;

EthernetClient client;

boolean printedPrompt = false;

namespace {
    // lowest pin number with LED attached for simulator usage
    int const pin_min = 3;
    // highest pin number with LED attached for simulator usage
    int const pin_max = 7;
    // pin with attached LED used to signal handling received byte
    int const pin_signal_rx = 8;
    // pin with attached LED used to signal step execution of PNVM
    int const pin_signal_loop = 9;
    // pin with attached LED used to signal fatal error with no possible
    // recovery
    int const pin_signal_error = 2;

    // global variables
    PNVMStringHead *input_message;
    // iterator for input_message
    PNVMStringIter inpiter;
    /* Guard for serial_interrupt() function to prevent multiple executions,
     * when one have not finished yet. */
    //volatile bool in_service = false;
    /* Flag set, when serial_interrupt() function collects a token terminated
     * with a newline, which is stored in input_message variable.
     * When set, it will be handled by main at the beginning of main loop
     * after the current execution of step ends.
     */
    volatile bool message_complete = false;

    /* Based on:
     *   http://forum.arduino.cc/index.php?PHPSESSID=nbpiqhal48hvk6p542sue2hgd7&topic=45076.msg326482#msg326482 */
    //void serial_interrupt() {
        /* Trick: since Serial I/O in interrupt driven,
         * we must reenable interrupts while in this Interrupt Service Routine.
         */
        /* But doing so will cause the routine to be called nestedly,
         * causing problems. */
        // So we mark we are already in service.

        /*
        // Already in service? Do nothing.
        if (in_service || message_complete) return;

        // You was not in service. Now you are.
        in_service = true;

        // let's fade out loop LED
        digitalWrite(pin_signal_loop, LOW);
        */

        /* Reenable interrupts, to allow Serial to work.
         * We do this only if in_service is false. */
        /*
        interrupts();

        // Allow serial to read at least one byte.
        while(!message_complete && Serial.available()) {
            // Blink led to signal Serial data arrived.
            digitalWrite(pin_signal_rx, !digitalRead(pin_signal_rx));

            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                if (pnvmValueItemCount(input_message) > 0) {
                    message_complete = true;
                }
            } else if (  c != '\r'
                      && (  pnvmValueItemCount(input_message) > 0
                         || !isblank(c)))
            {   // append character to input_message
                pnvmIterWrite(inpiter, c);
            }
            digitalWrite(pin_signal_rx, LOW);
        }

        // Job done.
        in_service = false;
    }
    */

    /* Each bit in *b* indexed by $i$ says, whether the LED attached to
     * pin $i + pin_min$ should be lit.
     */
    void set_leds(unsigned int b) {
        for (int i=0; i <= pin_max - pin_min; ++i) {
            digitalWrite(i + pin_min, b&(1 << i) ? HIGH:LOW);
        }
    }

    /* Debugging function printing free memory and available space left
     * in netTemplateCode array. */
    void signal_free_mem() {
        Serial.print("free memory: ");
        Serial.println(MEMBLOCK_GetFreeMem());
        Serial.print("free code space: ");
        Serial.println(NetTemplateCodeSize - primitiveCodeSize());
    }

    void handle_input() {
        while (client.available()) {
            char c = client.read();
            if (c == '\n' || c == '\r') {
                if (pnvmValueItemCount(input_message) > 0) {
                    if (!primitiveInputString(input_message)) {
                        server.print("failed to parse (error=");
                        server.print(getError());
                        server.println(')');
                        setError(ErrorSuccess);
                    }else {
                        server.print("appending");
                    }
                    server.print(" message of length: ");
                    server.println(pnvmValueItemCount(input_message));

                    // clean input_message buffer
                    pnvmValueDelete(input_message);
                    input_message = pnvmStringNew();
                    inpiter = pnvmValueIterator(input_message);
                    signal_free_mem();
                } else  {
		  if(!printedPrompt) {
		    server.print("pnos> ");
		  }
		  printedPrompt = true;
		}
            } else if (  c != '\r'
                      && (  pnvmValueItemCount(input_message) > 0
                         || !isblank(c)))
            {   // append character to input_message
                pnvmIterWrite(inpiter, c);
		Serial.print(c);
            }
            digitalWrite(pin_signal_rx, pnvmValueItemCount(input_message) % 2);
        }
    }

    // if platform made any output value, send it over serial line in text form
    void handle_output() {
        if (!primitiveOutputBufferEmpty() && !failed()) {
            PNVMStringHead *out = primitiveOutputPopMessage();
            PNVMStringIter i = pnvmValueIterator(out);
	    server.write("result: ");
	    String s = "";
            while (!pnvmIterAtEnd(i)) {
		char iter = pnvmIterNext(i); 
                Serial.print(iter);
		s = s + iter;
	    }
	    server.print(s);
	    if(!printedPrompt) {
	      server.print("\npnos> ");
	    }
	    Serial.println();
            pnvmValueDelete(out);
        }
    }

}

/* no return from this function
 * light up error diod, send the error via serial line
 * and do nothing from now on
 */
void dieWith(int code) {
    Serial.print("fatal error=");
    Serial.print(code);
    Serial.println(", terminating");
    digitalWrite(pin_signal_error, HIGH);
    signal_free_mem();
    // rest in peace
    while (true) {};
}


void setup() {
    Ethernet.begin(mac, ip, myDNS, gateway, subnet);
    server.begin();
  
    char const * const places[] = PLATFORM_PLACES;
    // initialize heap
    MEMBLOCK_InitManager(1, &block_workspace);
    MEMBLOCK_InitBag(0,     // bag id
            AllocationBlockSize,
            TotalAllocationBlocks,
            block_heap);

    Serial.begin(9600);
    // initialize led diods
    for (int p=pin_min; p <= pin_max; ++p) {
        pinMode(p, OUTPUT);
    }
    pinMode(pin_signal_rx, OUTPUT);
    pinMode(pin_signal_loop, OUTPUT);
    pinMode(pin_signal_error, OUTPUT);
    set_leds(0);

    while(!Serial) {}
    /* In order for this to work, Rx pin 0 must be connected with interrupt 0
     * pin -- pin num. 2. */
    //attachInterrupt(0, serial_interrupt, CHANGE);
    // wait for input
    //while (!Serial.available()) {}

    // set up simulator
    initializeModule();
    if (primitiveMakePlatformTemplateplaceCountplacessymbolscodeLengthtransitions(
                PLATFORM_NAME,
                PLATFORM_PLACE_COUNT,
                places,
                PLATFORM_SYMBOLS,
                PLATFORM_CODE_LENGTH,
                PLATFORM_TRANSITIONS) != ErrorSuccess)
    {
        Serial.println("failed to make platform");
        dieWith(getError());
    }else {
        PNVMNetInstHead * inst = primitiveInstantiateTemplate(0);
        if (!inst) {
            Serial.println("failed to instantiate");
            dieWith(getError());
        }
        input_message = pnvmStringNew();
        inpiter = pnvmValueIterator(input_message);
    }
    signal_free_mem();
}

void loop() {
    client = server.available();
    
    if(client) {
      if(!alreadyConnected) {
	client.flush();
	Serial.println("We have new client");
	client.println("Hello, user!");
	client.print("pnos> ");
	printedPrompt = true;
	alreadyConnected = true;
      }
    }
  
    handle_input();
    if (!failed()) {
        handle_output();
    }
    if (!failed()) {
        digitalWrite(pin_signal_loop, primitiveStep() % 2 ? HIGH:LOW);
    }
    if (failed()) {
        dieWith(getError());
    }
    printedPrompt = false;
}