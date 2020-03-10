//
// Albany 3.0: Copyright 2016 National Technology & Engineering Solutions of
// Sandia, LLC (NTESS). This Software is released under the BSD license detailed
// in the file license.txt in the top-level Albany directory.
//

// @HEADER

#ifndef DISPLAYTABLE_HPP_
#define DISPLAYTABLE_HPP_

/**
 *  \file DisplayTable.hpp
 *
 *  \brief
 */

#include <ostream>
#include <utility>
#include <vector>

#include "string.hpp"

namespace util {

class DisplayTable
{
 public:
  template <typename... Args>
  void
  addRow(Args... args);

  std::ostream&
  write(std::ostream& strm);
  std::ostream&
  writeCSV(std::ostream& strm, char const delim = ',');

 private:
  typedef std::vector<string> TableRow;

  template <class T, typename... Args>
  void
  addRow(TableRow& row, const T& val, Args... args);

  template <class T>
  void
  addRow(TableRow& row, const T& val);

  std::vector<TableRow> rows_;
};

template <typename... Args>
inline void
DisplayTable::addRow(Args... args)
{
  // TODO, when compiler allows, replace following with this for performance:
  // rows_.emplace_back();
  rows_.push_back(TableRow());
  addRow(rows_.back(), args...);
}

template <class T, typename... Args>
inline void
DisplayTable::addRow(TableRow& row, const T& val, Args... args)
{
  row.push_back(to_string(val));
  addRow(row, args...);
}

template <class T>
inline void
DisplayTable::addRow(TableRow& row, const T& val)
{
  row.push_back(to_string(val));
}

}  // namespace util

#endif  // DISPLAYTABLE_HPP_
