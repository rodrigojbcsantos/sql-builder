#pragma once

#include "model.hpp"
#include <string>
#include "col.hpp"
#include <vector>

namespace boosql {

class update_model : public model
{
public:
    update_model(shared_ptr<adapter> adapter) : model(adapter) {}
    update_model& update(const std::string& table_name) {
        _table_name = table_name;
        return *this;
    }

    template <typename T>
    update_model& set(const std::string& c, const T& data) {
        _columns.push_back(col(c)("=").val(data));
        return *this;
    }

    template <typename T>
    update_model& operator()(const std::string& c, const T& data) {
        return set(c, data);
    }

    update_model& and_where(const string & condition)
    {
        model::and_where(condition);
        return *this;
    }

    update_model& and_where(const col & condition)
    {
        model::and_where(condition);
        return *this;
    }

    update_model& or_where(const string & condition)
    {
        model::or_where(condition);
        return *this;
    }

    update_model& or_where(const col & condition)
    {
        model::or_where(condition);
        return *this;
    }

    update_model& quote(std::function<void(update_model& model)> callback)
    {
        model::quote<update_model>(callback, *this);
        return *this;
    }

    update_model& where(const string& condition) {
        model::where(condition);
        return *this;
    }

    update_model& where(const col& condition) {
        model::where(condition);
        return *this;
    }

    virtual const string & table_name()
    {
        return _table_name;
    }

    virtual const string& str() override {
        _sql.clear();
        _sql.append("UPDATE ");
        _sql.append(_adapter->quote_field(_table_name));
        _sql.append(" SET ");
        size_t size = _columns.size();
        for(size_t i = 0; i < size; ++i) {
            _sql.append(_columns[i].str(_adapter.get(), table_name()));
            if(i < size - 1) {
                _sql.append(", ");
            }
        }
        _sql.append(where_str());

        return _sql;
    }

    update_model& reset() {
        model::reset();
        _table_name.clear();
        _columns.clear();
        return *this;
    }
    friend inline std::ostream& operator << (std::ostream& out, update_model& mod) {
        out << mod.str();
        return out;
    }

protected:
    vector<col> _columns;
    string _table_name;
};

}