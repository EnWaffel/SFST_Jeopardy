#include "WebInterface.h"
#include <boost/network.hpp>

static std::string SendRequest(const std::string& data)
{
    boost::network::http::client::request request_("http://api.projectp.eu/sfst/jeopardy");

    std::cout << "data: " << data << std::endl;

    request_ << boost::network::header("Connection", "close");
    request_ << boost::network::header("Content-Type", "application/json");
    request_ << boost::network::header("Content-Length", std::to_string(data.length()));
    request_ << boost::network::body(data);

    boost::network::http::client client_;
    boost::network::http::client::response response_ = client_.post(request_);
    std::string content = boost::network::http::body(response_);

    std::cout << "response: " << content << std::endl;

    return content;
}

static std::string ConvertString(const std::wstring& str)
{
    std::string result;
    for (wchar_t c : str)
    {
        if (c == L'ß' || c == L'ü' || c == L'Ü' || c == L'ö' || c == L'Ö' || c == L'ä' || c == L'Ä' || c == L'\"')
        {
            switch (c)
            {
            case L'ß': result.append("$s"); break;
            case L'ü': result.append("$u"); break;
            case L'Ü': result.append("$U"); break;
            case L'ö': result.append("$o"); break;
            case L'Ö': result.append("$O"); break;
            case L'ä': result.append("$a"); break;
            case L'Ä': result.append("$A"); break;
            case L'\"': result.append("\\\""); break;
            }
        }
        else
        {
            char _c = (char)c;
            result.push_back(_c);
        }
    }
    return result;
}

void WebInterface::Start()
{
    try
    {
        SendRequest("{\"action\":\"start\"}");
    }
    catch (std::exception& e)
    {
        std::cout << "WebInterface exception: " << e.what() << std::endl;
    }
}

void WebInterface::End()
{
    try
    {
        SendRequest("{\"action\":\"end\"}");
    }
    catch (std::exception& e)
    {
        std::cout << "WebInterface exception: " << e.what() << std::endl;
    }
}

void WebInterface::SetContestants(const std::vector<std::string>& names)
{
    std::string data = "{\"action\": \"set_contestants\", \"names\": [";

    int i = 0;
    for (const std::string& name : names)
    {
        data += "\"" + name + "\"";
        if (i < names.size() - 1)
        {
            data += ",";
        }
        i++;
    }

    data += "]}";

    SendRequest(data);
}

void WebInterface::UpdatePoints(const std::vector<int>& orderedPoints)
{
    std::string data = "{\"action\": \"update_points\", \"points\": [";

    int i = 0;
    for (int points : orderedPoints)
    {
        data += std::to_string(points);
        if (i < orderedPoints.size() - 1)
        {
            data += ",";
        }
        i++;
    }

    data += "]}";

    SendRequest(data);
}

void WebInterface::EnableQuestion(const std::wstring& _question, const std::wstring& _answer)
{
    std::string question = ConvertString(_question);
    std::string answer = ConvertString(_answer);

    try
    {
        SendRequest("{\"action\":\"question_enable\",\"question\":\"" + question + "\",\"answer\":\"" + answer + "\"}");
    }
    catch (std::exception& e)
    {
        std::cout << "WebInterface exception: " << e.what() << std::endl;
    }
}

void WebInterface::DisableQuestion()
{
    try
    {
        SendRequest("{\"action\":\"question_disable\"}");
    }
    catch (std::exception& e)
    {
        std::cout << "WebInterface exception: " << e.what() << std::endl;
    }
}
