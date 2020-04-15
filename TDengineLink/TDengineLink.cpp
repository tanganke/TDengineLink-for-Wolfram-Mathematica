// Author: Tang Anke
// Date: 2020-Apr
#include <taos.h>
#include <memory>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <mathlink.h>

using std::string;
using std::vector;

vector<TAOS *> connections;

extern "C"
{
    extern MLINK stdlink;
    void TDengineConnect(char *ip, char *user, char *pass, char *db, int port);
    void TDengineClose(int id);
    void TDengineQuery(int id, char *sqlstr);
    void TDengineServerInfo(int id);
    void TDengineClientInfo();
}

void Failure(const char *msg)
{
    MLPutFunction(stdlink, "Failure", 2);
    MLPutString(stdlink, "error");
    MLPutFunction(stdlink, "Association", 1);
    MLPutFunction(stdlink, "Rule", 2);
    MLPutString(stdlink, "MessageTemplate");
    MLPutString(stdlink, msg);
    MLEndPacket(stdlink);
}

void TDengineConnect(char *ip, char *user, char *pass, char *db, int port)
{

    auto taos = taos_connect(ip, user, pass, db, port);
    if (taos == nullptr)
    // failure
    {
        Failure("can't connect.");
        return;
    }
    size_t pos = 0;
    for (auto &i : connections)
    {
        if (i == nullptr)
        {
            i = taos;
            MLPutFunction(stdlink, "TDengineDescriptor", 1);
            MLPutInteger64(stdlink, pos);
            goto RETURN;
        }
        pos++;
    }
    connections.push_back(taos);
    MLPutFunction(stdlink, "TDengineDescriptor", 1);
    MLPutInteger64(stdlink, connections.size() - 1);
RETURN:
    MLEndPacket(stdlink);
}

void TDengineServerInfo(int id)
{
    if (id >= connections.size() || id < 0 || connections[id] == nullptr)
    // failure
    {
        Failure("no such database connection.");
        return;
    }

    MLPutString(stdlink, taos_get_server_info(connections[id]));
    MLEndPacket(stdlink);
    return;
}

void TDengineClientInfo()
{
    MLPutString(stdlink, taos_get_client_info());
    MLEndPacket(stdlink);
    return;
}

void TDengineClose(int id)
{
    if (id >= connections.size() || id < 0)
    // failure
    {
        Failure("no such database connection.");
        return;
    }
    if (connections[id] == nullptr)
    // failure
    {
        Failure("this database connection has already been closed.");
        return;
    }
    taos_close(connections[id]);
    connections[id] = nullptr;
    MLPutSymbol(stdlink, "Null");
    MLEndPacket(stdlink);
}

void TDengineQuery(int id, char *sqlstr)
{
    if (taos_query(connections[id], sqlstr) != 0)
    // error
    {
        char *estr = taos_errstr(connections[id]);
        Failure(estr);
        // free(estr);
        return;
    }
    else if (id >= connections.size() || id < 0 || connections[id] == nullptr)
    {
        Failure("no such database connection.");
        return;
    }
    else
    {
        TAOS_RES *res = taos_use_result(connections[id]);
        if (res == nullptr)
        {
            MLPutSymbol(stdlink, "Null");
            MLEndPacket(stdlink);
            return;
        }

        int cols = taos_num_fields(res);
        if(cols==0)
        {
            MLPutFunction(stdlink,"Success",2);
            MLPutString(stdlink,"ExecutionCompleted");
            MLPutFunction(stdlink,"Association",2);
            MLPutFunction(stdlink,"Rule",2);
            MLPutString(stdlink,"SQL");
            MLPutString(stdlink,sqlstr);
            MLPutFunction(stdlink,"Rule",2);
            MLPutString(stdlink,"TimeStamp");
            MLPutFunction(stdlink,"DateString",0);
            MLEndPacket(stdlink);
            return;
        }
        TAOS_FIELD *fields = taos_fetch_fields(res);
        /*
         <| "ColumnName"->{col1,col2,...,coln},
            "ColumnType"->{type1,...},
            "Data"->{{...},{...}...} |>
        */
        MLPutFunction(stdlink, "Association", 3);
        MLPutFunction(stdlink, "Rule", 2);
        MLPutString(stdlink, "ColumnName");
        MLPutFunction(stdlink, "List", cols);
        for (int i = 0; i < cols; ++i)
        {
            MLPutString(stdlink, fields[i].name);
        }

        MLPutFunction(stdlink, "Rule", 2);
        MLPutString(stdlink, "ColumnType");
        MLPutFunction(stdlink, "List", cols);

        for (int i = 0; i < cols; ++i)
        {
            switch (fields[i].type)
            {
            case TSDB_DATA_TYPE_NULL:
                MLPutString(stdlink, "Null");
                break;
            case TSDB_DATA_TYPE_BOOL:
                MLPutString(stdlink, "Boolean");
                break;
            case TSDB_DATA_TYPE_TINYINT:
                MLPutString(stdlink, "Integer8");
                break;
            case TSDB_DATA_TYPE_SMALLINT:
                MLPutString(stdlink, "Integer16");
                break;
            case TSDB_DATA_TYPE_INT:
                MLPutString(stdlink, "Integer32");
                break;
            case TSDB_DATA_TYPE_BIGINT:
                MLPutString(stdlink, "Integer64");
                break;
            case TSDB_DATA_TYPE_FLOAT:
                MLPutString(stdlink, "Real32");
                break;
            case TSDB_DATA_TYPE_DOUBLE:
                MLPutString(stdlink, "Real64");
                break;
            case TSDB_DATA_TYPE_BINARY:
                MLPutString(stdlink, "CString");
                break;
            case TSDB_DATA_TYPE_TIMESTAMP:
                MLPutString(stdlink, "TimeStamp");
                break;
            case TSDB_DATA_TYPE_NCHAR:
                MLPutString(stdlink, "MultibyteString");
                break;
            default:
                Failure("unknown data type.");
                return;
            }
        }

        MLPutFunction(stdlink, "Rule", 2);
        MLPutString(stdlink, "Data");
        
        MLPutFunction(stdlink,"Partition",2);
        MLPutFunction(stdlink,"Flatten",1);
        MLPutFunction(stdlink, "List", 2);
        TAOS_ROW row;
        while(row=taos_fetch_row(res))
        {
            MLPutFunction(stdlink, "List", cols);
            for (int i = 0; i < cols; ++i)
            {
                if(row[i]==nullptr)
                {
                    MLPutSymbol(stdlink,"Null");
                    continue;
                }

                switch (fields[i].type)
                {
                case TSDB_DATA_TYPE_NULL:
                    MLPutSymbol(stdlink, "Null");
                    break;
                case TSDB_DATA_TYPE_BOOL:
                    MLPutSymbol(stdlink, *((bool *)row[i]) ? "True" : "False");
                    break;
                case TSDB_DATA_TYPE_TINYINT:
                    MLPutInteger8(stdlink, *((unsigned char *)row[i]));
                    break;
                case TSDB_DATA_TYPE_SMALLINT:
                    MLPutInteger16(stdlink, *((short *)row[i]));
                    break;
                case TSDB_DATA_TYPE_INT:
                    MLPutInteger32(stdlink, *((int *)row[i]));
                    break;
                case TSDB_DATA_TYPE_BIGINT:
                    MLPutInteger64(stdlink, *((long *)row[i]));
                    break;
                case TSDB_DATA_TYPE_FLOAT:
                    MLPutReal32(stdlink, *((float *)row[i]));
                    break;
                case TSDB_DATA_TYPE_DOUBLE:
                    MLPutReal64(stdlink, *((double *)row[i]));
                    break;
                case TSDB_DATA_TYPE_BINARY:
                case TSDB_DATA_TYPE_NCHAR:
                    MLPutString(stdlink, (char *)row[i]);
                    break;
                case TSDB_DATA_TYPE_TIMESTAMP:
                    MLPutInteger64(stdlink,*((long*)row[i]));
                    break;
                default:
                    Failure("unknown data type.");
                    return;
                }
            }
            MLPutFunction(stdlink,"List",2);
        }
        MLPutSymbol(stdlink,"Nothing");
        MLPutSymbol(stdlink,"Nothing");
        MLPutInteger32(stdlink,cols);
        MLEndPacket(stdlink);
        taos_free_result(res);
        return;
    }
}
