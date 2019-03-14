#pragma once

#include <vector>
#include <string>
#include <functional>

#include "col.hpp"
#include "model.hpp"

namespace boosql {

using namespace std;

class select_model : public model
{
public:
    select_model(shared_ptr<adapter> adapter): model(adapter) {}
    virtual ~select_model() {}

    template <typename... Args>
    select_model& select(const col& c, Args&&... columns) {
        _select.push_back(c);
        select(columns...);
        return *this;
    }

    // for recursion
    select_model& select() {
        return *this;
    }

    select_model& from(const string& table_name) {
        _table_name = table_name;
        return *this;
    }
    
    select_model& and_where(const string & condition)
    {
        model::and_where(condition);
        return *this;
    }

    select_model& and_where(const col & condition)
    {
        model::and_where(condition);
        return *this;
    }

    select_model& or_where(const string & condition)
    {
        model::or_where(condition);
        return *this;
    }

    select_model& or_where(const col & condition)
    {
        model::or_where(condition);
        return *this;
    }

    select_model& quote(std::function<void(select_model& model)> callback)
    {
        model::quote<select_model>(callback, *this);
        return *this;
    }

    select_model& where(const string& condition) {
        model::where(condition);
        return *this;
    }

    select_model& where(const col& condition) {
        model::where(condition);
        return *this;
    }

    template <typename... Args>
    select_model& group_by(const col& c, Args&&...columns) {
        _groupby_columns.push_back(c);
        group_by(columns...);
        return *this;
    }

    // for recursion
    select_model& group_by() {
        return *this;
    }

    select_model& having(const string& condition) {
        _having_condition.push_back(col("")(condition));
        return *this;
    }

    select_model& having(const col& condition) {
        _having_condition.push_back(condition);
        return *this;
    }

    select_model& order_by(const col& order_by) {
        _order_by.push_back(order_by);
        return *this;
    }

    template <typename T>
    select_model& limit(const T& limit) {
        _limit = to_string(limit);

        return *this;
    }

    select_model& page(const int& page, const int& page_size) {
        offset((page - 1) * page_size);
        limit(page_size);
        return *this;
    }

    template <typename T>
    select_model& offset(const T& offset) {
        _offset = to_string(offset);
        return *this;
    }

    const string & table_name()
    {
        return _table_name;
    }

    const string& str() override
    {
        _sql.clear();
        _sql.append("SELECT ");
        _sql.append(select_str());
        _sql.append(" FROM ");
        _sql.append(_table_name);
        _sql.append(where_str());
        _sql.append(group_by_str());
        _sql.append(having_str());
        _sql.append(order_by_str());
        if(!_limit.empty()) {
            _sql.append(" LIMIT ");
            _sql.append(_limit);
        }
        if(!_offset.empty()) {
            _sql.append(" OFFSET ");
            _sql.append(_offset);
        }
        return _sql;
    }

    string order_by_str()
    {
        string ret;
        auto size = _order_by.size();
        if (size > 0) {
            ret.append(" ORDER BY ");
            for (size_t i = 0; i < size; ++i) {
                ret.append(_order_by[i].str(_adapter.get(), _table_name));
                if(i < size - 1) {
                    _sql.append(", ");
                }
            }
        }

        return ret;
    }

    string having_str()
    {
        string ret;
        auto size = _having_condition.size();
        if(size > 0) {
            ret.append(" HAVING ");
            for(size_t i = 0; i < size; ++i) {
                ret.append(_having_condition[i].str(_adapter.get(), _table_name));
                if(i < size - 1) {
                    _sql.append(" ");
                }
            }
        }

        return ret;
    }

    string group_by_str()
    {
        string ret;
        auto size = _groupby_columns.size();
        if(!_groupby_columns.empty()) {
            ret.append(" GROUP BY ");
            for(size_t i = 0; i < size; ++i) {
                ret.append(_groupby_columns[i].str(_adapter.get(), _table_name));
                if(i < size - 1) {
                    ret.append(", ");
                }
            }
        }

        return ret;
    }


    string select_str()
    {
        string ret = "";
        int count = 0;
        for (auto i = _select.begin(); i != _select.end(); ++i) {
            count++;
            ret.append((*i).str(_adapter.get(), _table_name));
            if (count < _select.size()) {
                ret.append(", ");
            }
        }

        return ret;
    }

    select_model& reset() {
        model::reset();
        _table_name.clear();
        _select.clear();
        _groupby_columns.clear();
        _having_condition.clear();
        _order_by.clear();
        _limit.clear();
        _offset.clear();
        return *this;
    }
    friend inline ostream& operator<< (ostream& out, select_model& mod) {
        out<<mod.str();
        return out;
    }

private:
    string _table_name;
    vector<col> _select;
    vector<col> _groupby_columns;
    vector<col> _having_condition;
    vector<col> _order_by;
    string _limit;
    string _offset;
};

}