#pragma once

namespace Luna {
  namespace System {
    /**
     * \brief Get number of available processors.
     */
    unsigned int getProcessorCount();

    /**
     * \brief Get amount of used physical memory in bytes.
     */
    unsigned long getPhysicalMemoryUsage();

    /**
     * \brief Get amount of used virtual memory in bytes.
     */
    unsigned long getVirtualMemoryUsage();

    unsigned long getTotalMemorySize();
  }
}
