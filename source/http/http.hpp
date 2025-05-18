#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <sys/stat.h>
#include "../server.hpp"
#include <regex>

std::unordered_map<int, std::string> _statu_msg = {
    {100,  "Continue"},
    {101,  "Switching Protocol"},
    {102,  "Processing"},
    {103,  "Early Hints"},
    {200,  "OK"},
    {201,  "Created"},
    {202,  "Accepted"},
    {203,  "Non-Authoritative Information"},
    {204,  "No Content"},
    {205,  "Reset Content"},
    {206,  "Partial Content"},
    {207,  "Multi-Status"},
    {208,  "Already Reported"},
    {226,  "IM Used"},
    {300,  "Multiple Choice"},
    {301,  "Moved Permanently"},
    {302,  "Found"},
    {303,  "See Other"},
    {304,  "Not Modified"},
    {305,  "Use Proxy"},
    {306,  "unused"},
    {307,  "Temporary Redirect"},
    {308,  "Permanent Redirect"},
    {400,  "Bad Request"},
    {401,  "Unauthorized"},
    {402,  "Payment Required"},
    {403,  "Forbidden"},
    {404,  "Not Found"},
    {405,  "Method Not Allowed"},
    {406,  "Not Acceptable"},
    {407,  "Proxy Authentication Required"},
    {408,  "Request Timeout"},
    {409,  "Conflict"},
    {410,  "Gone"},
    {411,  "Length Required"},
    {412,  "Precondition Failed"},
    {413,  "Payload Too Large"},
    {414,  "URI Too Long"},
    {415,  "Unsupported Media Type"},
    {416,  "Range Not Satisfiable"},
    {417,  "Expectation Failed"},
    {418,  "I'm a teapot"},
    {421,  "Misdirected Request"},
    {422,  "Unprocessable Entity"},
    {423,  "Locked"},
    {424,  "Failed Dependency"},
    {425,  "Too Early"},
    {426,  "Upgrade Required"},
    {428,  "Precondition Required"},
    {429,  "Too Many Requests"},
    {431,  "Request Header Fields Too Large"},
    {451,  "Unavailable For Legal Reasons"},
    {501,  "Not Implemented"},
    {502,  "Bad Gateway"},
    {503,  "Service Unavailable"},
    {504,  "Gateway Timeout"},
    {505,  "HTTP Version Not Supported"},
    {506,  "Variant Also Negotiates"},
    {507,  "Insufficient Storage"},
    {508,  "Loop Detected"},
    {510,  "Not Extended"},
    {511,  "Network Authentication Required"}
};

std::unordered_map<std::string, std::string> _mime_msg = {
    {".aac",        "audio/aac"},
    {".abw",        "application/x-abiword"},
    {".arc",        "application/x-freearc"},
    {".avi",        "video/x-msvideo"},
    {".azw",        "application/vnd.amazon.ebook"},
    {".bin",        "application/octet-stream"},
    {".bmp",        "image/bmp"},
    {".bz",         "application/x-bzip"},
    {".bz2",        "application/x-bzip2"},
    {".csh",        "application/x-csh"},
    {".css",        "text/css"},
    {".csv",        "text/csv"},
    {".doc",        "application/msword"},
    {".docx",       "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot",        "application/vnd.ms-fontobject"},
    {".epub",       "application/epub+zip"},
    {".gif",        "image/gif"},
    {".htm",        "text/html"},
    {".html",       "text/html"},
    {".ico",        "image/vnd.microsoft.icon"},
    {".ics",        "text/calendar"},
    {".jar",        "application/java-archive"},
    {".jpeg",       "image/jpeg"},
    {".jpg",        "image/jpeg"},
    {".js",         "text/javascript"},
    {".json",       "application/json"},
    {".jsonld",     "application/ld+json"},
    {".mid",        "audio/midi"},
    {".midi",       "audio/x-midi"},
    {".mjs",        "text/javascript"},
    {".mp3",        "audio/mpeg"},
    {".mpeg",       "video/mpeg"},
    {".mpkg",       "application/vnd.apple.installer+xml"},
    {".odp",        "application/vnd.oasis.opendocument.presentation"},
    {".ods",        "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt",        "application/vnd.oasis.opendocument.text"},
    {".oga",        "audio/ogg"},
    {".ogv",        "video/ogg"},
    {".ogx",        "application/ogg"},
    {".otf",        "font/otf"},
    {".png",        "image/png"},
    {".pdf",        "application/pdf"},
    {".ppt",        "application/vnd.ms-powerpoint"},
    {".pptx",       "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar",        "application/x-rar-compressed"},
    {".rtf",        "application/rtf"},
    {".sh",         "application/x-sh"},
    {".svg",        "image/svg+xml"},
    {".swf",        "application/x-shockwave-flash"},
    {".tar",        "application/x-tar"},
    {".tif",        "image/tiff"},
    {".tiff",       "image/tiff"},
    {".ttf",        "font/ttf"},
    {".txt",        "text/plain"},
    {".vsd",        "application/vnd.visio"},
    {".wav",        "audio/wav"},
    {".weba",       "audio/webm"},
    {".webm",       "video/webm"},
    {".webp",       "image/webp"},
    {".woff",       "font/woff"},
    {".woff2",      "font/woff2"},
    {".xhtml",      "application/xhtml+xml"},
    {".xls",        "application/vnd.ms-excel"},
    {".xlsx",       "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml",        "application/xml"},
    {".xul",        "application/vnd.mozilla.xul+xml"},
    {".zip",        "application/zip"},
    {".3gp",        "video/3gpp"},
    {".3g2",        "video/3gpp2"},
    {".7z",         "application/x-7z-compressed"}
};


class Util
{
    public:
        //字符串分割函数,将src字符串按照sep字符进行分割，得到的各个字串放到arry中，最终返回字串的数量
        static size_t Split(const std::string &src, const std::string &sep, std::vector<std::string> *array)
        {
            size_t offset = 0;
            // 如果有10个字符，offset是查找的起始位置，范围应该是0~9，offset==10就代表已经越界了
            while (offset < src.size())
            {
                size_t pos = src.find(sep, offset);//在src字符串偏移量offset处，开始向后查找sep字符/字串，返回查找到的位置
                if(pos == std::string::npos)//没有找到特定的字符
                {
                    if(pos == src.size())break;
                    //将剩余的部分当作一个字串，放入arry中
                    array->push_back(src.substr(offset));
                    return array->size();
                }
                if(offset == pos)//当前字串是一个空的，没有内容
                {
                    offset = pos + sep.size();
                    continue;
                }
                array->push_back(src.substr(offset, pos - offset));
                offset = pos + sep.size();   
            }            
            return array->size();
            //eg:/dadafds/45645/../qqqq ----->Split result: 'dadafds','45645','..','qqqq'
        }

        //读取文件的所有内容，将读取的内容放到一个string中
        static bool ReadFile(const std::string &filename, std::string *buf)
        {
            std::ifstream ifs(filename, std::ios::binary);
            if(ifs.is_open() == false)
            {
                printf("OPEN %s FILE FAILED!", filename.c_str());
                return false;
            }
            size_t fsize = 0;
            ifs.seekg(0, ifs.end); //跳转读写位置到末尾
            fsize = ifs.tellg();   //获取当前读写位置相对于起始位置的偏移量，从末尾偏移刚好就是文件大小
            ifs.seekg(0, ifs.beg); //跳转到起始位置
            buf->resize(fsize);    //开辟文件大小的空间
            ifs.read(&(*buf)[0], fsize);//读fsize大小内容到buf中
            if(ifs.good() == false)
            {
                printf("READ %s FILE FAILED!", filename.c_str());
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }

        //向文件写入数据
        static bool WriteFile(const std::string &filename, std::string &buf)
        {
            std::ofstream ofs(filename, std::ios::binary | std::ios::trunc);
            if(ofs.is_open() == false)
            {
                printf("OPEN %s FILE FAILED!!", filename.c_str());
                return false;
            }
            ofs.write(buf.c_str(), buf.size());
            if(ofs.good() == false)
            {
                printf("WRITE %s FILE FAILED", filename.c_str());
                ofs.close();
                return false;
            }
            ofs.close();
            return true;       
        }


        //URL编码，避免URL中资源路径与查询字符串中的特殊字符与HTTP请求中特殊字符产生歧义
        //编码格式：将特殊字符的ascii值，转换为两个16进制字符，前缀%   C++ -> C%2B%2B
        //  不编码的特殊字符： RFC3986文档规定 . - _ ~ 字母，数字属于绝对不编码字符
        //RFC3986文档规定，编码格式 %HH 
        //W3C标准中规定，查询字符串中的空格，需要编码为+， 解码则是+转空格
        static std::string UrlEncode(const std::string url, bool convert_space_to_plus)
        {
            std::string res;
            for(auto &c : url)
            {
                if(c == '.' || c == '~' || c == '_' || c == '-' || isalnum(c))
                {
                    res += c;
                    continue;
                }
                if(c == ' ' && convert_space_to_plus)
                {
                    res += '+';
                    continue;
                }
                //剩下的字符都是需要编码成为 %HH 格式
                char tmp[4] = {0};
                snprintf(tmp, 4, "%%%02X", c);
                res += tmp;
            }
            return res;
        }

        //URL解码
        static char HEXTOI(char c)
        {
            if(c >= '0' && c <= '9'){return c - '0';}
            //+10 是将字母字符转换为对应的十六进制数值
            //无论是小写字母还是大写字母，它们在十六进制中的数值范围都是 10 到 15
            else if(c >= 'a' && c <= 'z'){return c - 'a' + 10;}
            else if(c >= 'A' && c <= 'Z'){return c - 'A' + 10;}
            return -1;
        }
        static std::string UrlDecode(const std::string url, bool convert_plus_to_space)
        {
            //遇到了%，则将紧随其后的2个字符，转换为数字，第一个数字左移4位(相当于*16），然后加上第二个数字
            //比如：+ -> 2b    %2b -> [(2 << 4) + 11]  (此时算出了+的ascall值)
            std::string res;
            for(int i = 0; i < url.size(); i++)
            {
                if(url[i] == '+' && convert_plus_to_space == true)
                {
                    res += ' ';
                    continue;
                }
                if(url[i] == '%' && (i+2) < url.size())
                {
                    char v1 = HEXTOI(url[i+1]);
                    char v2 = HEXTOI(url[i+2]);
                    char v = v1 * 16 + v2;
                    res += v;
                    i += 2;
                    continue;
                }
                res += url[i];
            }
            return res;
        }


        //响应状态码的描述信息获取
        static std::string StatuDesc(int statu)
        {
            auto it = _statu_msg.find(statu);
            if(it != _statu_msg.end())
            {
                return it->second;
            }
            return "Unknow";
        }

        //根据文件后缀名获取文件mime
        static std::string ExtMime(const std::string &filename)
        {
            // a.b.txt  先获取文件扩展名
            size_t pos = filename.find_last_of('.');
            if(pos == std::string::npos)
            {
                return "application/octet-stream";
            }
            //根据扩展名，获取mime
            std::string ext = filename.substr(pos);
            auto it = _mime_msg.find(ext);
            if(it == _mime_msg.end())
            {
                return "application/octet-stream";
            }
            return it->second;
        }

        //判断一个文件是否是一个目录
        static bool IsDirectory(const std::string &filename)
        {
            struct stat st;
            int ret = stat(filename.c_str(), &st);
            if(ret < 0)
            {
                return false;
            }
            return S_ISDIR(st.st_mode);
        }
        //判断一个文件是否是一个普通文件
        static bool IsRegular(const std::string &filename)
        {
            struct stat st;
            int ret = stat(filename.c_str(), &st);
            if(ret < 0)
            {
                return false;
            }
            return S_ISREG(st.st_mode);
        }

        //http请求的资源路径有效性判断
        // /index.html  --- 前边的/叫做相对根目录  映射的是某个服务器上的子目录
        // 想表达的意思就是，客户端只能请求相对根目录中的资源，其他地方的资源都不予理会
        // /../login, 这个路径中的..会让路径的查找跑到相对根目录之外，这是不合理的，不安全的
        static bool ValidPath(const std::string &path)
        {
            //思想：按照/进行路径分割，根据有多少子目录，计算目录深度，有多少层，深度不能小于0
            std::vector<std::string> subdir;
            Split(path, "/", &subdir);
            int level = 0;
            for(auto &dir : subdir)
            {
                if(dir == "..")
                {
                    level--;//任意一层走出相对根目录，就认为有问题
                    if(level < 0)
                    {
                        return false;
                    }
                    continue;
                }
                level++;
            }
            return true;
        }
};

class HttpRequest
{
    public:
        std::string _method;    //请求方法
        std::string _path;      //资源路径
        std::string _version;   //协议版本
        std::string _body;      //请求正文
        std::smatch _matches;   //资源路径的正则提取规则
        std::unordered_map<std::string, std::string> _headers; //头部字段
        std::unordered_map<std::string, std::string> _params;   //查询字符串

    public:
        HttpRequest(): _version("HTTP/1.1"){}
        //将请求对象的状态重置为初始状态
        void Reset()
        {
            _method.clear();
            _path.clear();
            _version.clear();
            _body.clear();
            std::smatch matches;
            _matches.swap(matches);//仿clear
            _headers.clear();
            _params.clear();
        }
  
        //插入头部字段
        void SetHeader(const std::string &key, const std::string &val)
        {
            _headers.insert(std::make_pair(key, val));
        }
        //判断是否存在指定头部字段
        bool HasHeader(const std::string &key) const
        {
            auto it = _headers.find(key);
            if(it == _headers.end())
            {
                return false;
            }
            return true;
        }
        //获取指定头部字段的值
        std::string GetHeader(const std::string &key) const 
        {
            auto it = _headers.find(key);
            if(it == _headers.end())
            {
                return "";
            }
            return it->second;
        }

        //插入查询字符串
        void SetParam(const std::string &key, const std::string &val)
        {
            _params.insert(std::make_pair(key, val));
        }
        //判断是否有某个指定的查询字符串
        bool HasParam(const std::string &key)
        {
            auto it = _params.find(key);
            if(it == _params.end())
            {
                return false;
            }
            return true;
        } 
        //获取指定的查询字符串
        std::string GetParam(const std::string &key) const 
        {
            auto it = _params.find(key);
            if(it == _params.end())
            {
                return "";
            }
            return it->second;
        }

        //获取正文长度
        size_t ContentLength() const
        {
            // Content-Length: 1234\r\n
            bool ret = HasHeader("Content-Length");
            if(ret == false)
            {
                return 0;
            }
            std::string clen = GetHeader("Content-Length");
            return std::stoi(clen);
        }

        //判断是否是短链接
        bool Close() const
        {
            // 没有Connection字段，或者有Connection但是值是close，则都是短链接，否则就是长连接
            if(HasHeader("Connection") == true && GetHeader("Connection") == "keep-alive")
            {
                return false;
            }
            return true;
        }
};

class HttpResponse
{
    public:
        int _statu;
        bool _redirect_flag;        //表示是否设置了重定向
        std::string _body;
        std::string _redirect_url;  //存储重定向的目标 URL
        std::unordered_map<std::string, std::string> _headers;

    public:
        HttpResponse(): _redirect_flag(false), _statu(200){}//200（表示成功）
        HttpResponse(int statu): _redirect_flag(false), _statu(statu){}

        void Reset()
        {
            _statu = 200;
            _redirect_flag = false;
            _body.clear();
            _redirect_url.clear();
            _headers.clear();
        }

        //插入头部字段
        void SetHeader(const std::string &key, const std::string &val)
        {
            _headers.insert(std::make_pair(key, val));
        }
        //判断是否存在指定头部字段
        bool HasHeader(const std::string &key) const
        {
            auto it = _headers.find(key);
            if(it == _headers.end())
            {
                return false;
            }
            return true;
        }
        //获取指定头部字段的值
        std::string GetHeader(const std::string &key)
        {
            auto it = _headers.find(key);
            if(it == _headers.end())
            {
                return "";
            }
            return it->second;
        }
        
        //设置响应正文和内容类型。
        void SetContent(const std::string &body, const std::string &type = "text/html")
        {
            _body = body;
            SetHeader("Content-Type", type);
        }

        //设置重定向响应，默认为 302（临时重定向）
        void Setdirect(const std::string &url, int statu = 302)
        {
            _statu = statu;
            _redirect_flag = true;
            _redirect_url = url;
        }

        //判断是否是短链接
        bool Close() 
        {
            // 没有Connection字段，或者有Connection但是值是close，则都是短链接，否则就是长连接
            if(HasHeader("Connection") == true && GetHeader("Connection") == "keep-alive")
            {
                return false;
            }
            return true;
        }

};

typedef enum
{
    RECV_HTTP_HEAD,
    RECV_HTTP_LINE,
    RECV_HTTP_BODY,
    RECV_HTTP_OVER,
    RECV_HTTP_ERROR
}HttpRecvStatu;

//上下文模块
#define MAX_LINE 8192
class HttpContext
{
    private:
        int _resp_statu;            //响应状态码
        HttpRecvStatu _recv_statu;  //当前接收及解析的阶段状态
        HttpRequest _request;       //已经解析得到的请求信息
    
    private:
        //请求行解析  
        bool ParseHttpLine(const std::string &line)
        {
            std::smatch matches;
            std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])(?:\n|\r\n)?", std::regex::icase);
            bool ret = std::regex_match(line, matches, e);
            if(ret == false)
            {
                _recv_statu = RECV_HTTP_ERROR;
                _resp_statu = 400;//BAD REQUEST
                return false;
            }
            //0 : GET /bitejiuyeke/login?user=xiaoming&pass=123123 HTTP/1.1
            //1 : GET
            //2 : /bitejiuyeke/login
            //3 : user=xiaoming&pass=123123
            //4 : HTTP/1.1
            //请求方法的获取
            _request._method = matches[1];
            std::transform(_request._method.begin(), _request._method.end(), _request._method.begin(), ::toupper);//将请求方法转换为大写
            //资源路径的获取，需要进行URL解码操作，但是不需要'+'转空格
            _request._path = Util::UrlDecode(matches[2], false);
            //协议版本的获取
            _request._version = matches[4];
            //查询字符串的获取与处理
            std::vector<std::string> query_string_arry;
            std::string query_string = matches[3];
            //查询字符串的格式 key=val&key=val....., 先以 & 符号进行分割，得到各个字串
            Util::Split(query_string, "&", &query_string_arry);
            //针对各个字串，以 = 符号进行分割，得到key 和val， 得到之后也需要进行URL解码
            for(auto &str : query_string_arry)
            {
                size_t pos = str.find("=");
                if(pos == std::string::npos)
                {
                    _recv_statu = RECV_HTTP_ERROR;
                    _resp_statu = 400;//BAD REQUEST
                    return false;
                }
                std::string key = Util::UrlDecode(str.substr(0, pos), true);
                std::string val = Util::UrlDecode(str.substr(pos + 1), true); 
                _request.SetParam(key, val);
            }
            return true;
        }
        //首行接收
        bool RecvHttpLine(Buffer *buf)
        {
            if(_recv_statu != RECV_HTTP_LINE) return false;
            //1. 获取一行数据，带有末尾的换行
            std::string line = buf->GetLineAndPop();
            //2. 需要考虑的一些要素：缓冲区中的数据不足一行(此时line:"")， 获取的一行数据超大
            if(line.size() == 0)
            {
                //缓冲区中的数据不足一行，则需要判断缓冲区的可读数据长度，如果很长了都不足一行，这是有问题的
                if(buf->ReadAbleSize() > MAX_LINE)
                {
                    _recv_statu = RECV_HTTP_ERROR;
                    _resp_statu = 414;//URL TOO LONG
                    return false;
                }
                //缓冲区中数据不足一行，但是也不多，就等等新数据的到来
                return true;
            }
            //有行，但请求行过长
            if(line.size() > MAX_LINE)
            {
                _recv_statu = RECV_HTTP_ERROR;
                _resp_statu = 414;//URL TOO LONG
                return false;
            }
            bool ret = ParseHttpLine(line);
            if(ret == false)
            {
                return false;
            }
            //首行处理完毕，进入头部获取阶段
            _recv_statu = RECV_HTTP_HEAD;
            return true;
        }

        //头部接收
        bool RecvHttpHead(Buffer *buf)
        {
            if(_recv_statu != RECV_HTTP_HEAD) return false;
            //一行一行取出数据，直到遇到空行为止， 头部的格式 key: val\r\nkey: val\r\n....
            while (1)
            {
                //1. 获取一行数据，带有末尾的换行
                std::string line = buf->GetLineAndPop();
                //2. 需要考虑的一些要素：缓冲区中的数据不足一行(此时line:"")， 获取的一行数据超大
                if(line.size() == 0)
                {
                    //缓冲区中的数据不足一行，则需要判断缓冲区的可读数据长度，如果很长了都不足一行，这是有问题的
                    if(buf->ReadAbleSize() > MAX_LINE)
                    {
                        _recv_statu = RECV_HTTP_ERROR;
                        _resp_statu = 414;//URL TOO LONG
                        return false;
                    }
                    //缓冲区中数据不足一行，但是也不多，就等等新数据的到来
                    return true;
                }
                //有行，但请求行过长
                if(line.size() > MAX_LINE)
                {
                    _recv_statu = RECV_HTTP_ERROR;
                    _resp_statu = 414;//URL TOO LONG
                    return false;
                }
                //如果处理到空行，说明头部处理结束
                if(line == "\n" || line == "\r\n")
                {
                    break;
                }
                bool ret = ParseHttpHead(line);
                if(ret == false)
                {
                    return false;
                }
            }
            //头部处理完毕，进入正文获取阶段
            _recv_statu == RECV_HTTP_BODY;
            return true;
        }

        //头部每一行的处理
        bool ParseHttpHead(std::string &line)
        {
            //key: val\r\nkey: val\r\n....
            if(line.back() == '\n') line.pop_back();//末尾是换行则去掉换行字符
            if(line.back() == '\r') line.pop_back();//末尾是回车则去掉回车字符
            size_t pos = line.find(": ");
            if(pos == std::string::npos)
            {
                _recv_statu = RECV_HTTP_ERROR;
                _resp_statu = 400;
                return false; 
            }
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 2);
            _request.SetHeader(key, val);
            return true;
        }

        //正文接收
         bool RecvHttpBody(Buffer *buf)
        {
            if(_recv_statu != RECV_HTTP_BODY) return false;
            //1. 获取正文长度
            size_t content_length = _request.ContentLength();
            if(content_length == 0)
            {
                //没有正文，则请求接收解析完毕
                _recv_statu = RECV_HTTP_OVER;
                return true;
            }
            //2. 当前已经接收了多少正文,其实就是往  _request._body 中放了多少数据了
            size_t real_len = content_length - _request._body.size();//实际还需要接收的正文长度
            //3. 接收正文放到body中，但是也要考虑当前缓冲区中的数据，是否是全部的正文
                //  3.1 缓冲区中数据，包含了当前请求的所有正文，则取出所需的数据
            if(buf->ReadAbleSize() >= real_len)
            {
                _request._body.append(buf->ReadPosition(), real_len);
                buf->MoveReadOffset(real_len);
                _recv_statu = RECV_HTTP_OVER;
                return true;
            }
                //  3.2 缓冲区中数据，无法满足当前正文的需要，数据不足，取出数据，然后等待新数据到来
            _request._body.append(buf->ReadPosition(), buf->ReadAbleSize());
            buf->MoveReadOffset(buf->ReadAbleSize());
            return true;
        }

    public:
        HttpContext(): _resp_statu(200), _recv_statu(RECV_HTTP_LINE){}

        void ReSet()
        {
            _resp_statu = 200;
            _recv_statu = RECV_HTTP_LINE;
            _request.Reset();
        }

        int RespStatu(){return _resp_statu;}
        HttpRecvStatu RecvStatu(){return _recv_statu;}
        HttpRequest &Request(){return _request;}

        //接收并解析HTTP请求
        void RecvHttpRequest(Buffer *buf)
        {
            //不同的状态，做不同的事情，但是这里不要break， 因为处理完请求行后，应该立即处理头部，而不是退出等新数据
            switch(_recv_statu)
            {
                case RECV_HTTP_LINE: RecvHttpLine(buf);
                case RECV_HTTP_HEAD: RecvHttpHead(buf);
                case RECV_HTTP_BODY:  RecvHttpBody(buf);
            }
            return;
        }
};