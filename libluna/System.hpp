#pragma once

#include <list>

#include <libluna/String.hpp>

namespace Luna {
  /**
   * @brief Get information about system resources and capabilities.
   */
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

    std::list<String> getAssetFiles();
  } // namespace System
} // namespace Luna
