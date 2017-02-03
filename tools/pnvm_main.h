/******************************************************************************
 * These are callbacks for pnvm_plugin to the main interpreter program
 * providing interaction with outer world.
 *
 * We won't be simulating anything with these tools, so we don't
 * provide any functionality.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */

#ifndef PNVM_H
#define PNVM_H

#include <log4cxx/logger.h>

inline int digitalRead(int) { return 0; }
/*!
 * Set or reset pin.
 */
inline void digitalWrite(int, int) {}

/*!
 * Get current time in milliseconds.
 */
inline unsigned long millis() { return 0; }

extern log4cxx::LoggerPtr memlog;
extern log4cxx::LoggerPtr exlog;
extern log4cxx::LoggerPtr iolog;
extern log4cxx::LoggerPtr parselog;

#endif /* end of include guard: PNVM_H */
