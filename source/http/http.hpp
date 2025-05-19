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

#define DEFALT_TIMEOUT 10

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
                    if(pos == src.size()) break;
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
        static bool WriteFile(const std::string &filename, const std::string &buf)
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
                if(c == '.' || c == '-' || c == '_' || c == '~' || isalnum(c))
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
            _version = "HTTP/1.1";
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
        bool HasParam(const std::string &key) const 
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
            //std::cout << "首行解析完" << std::endl;
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
            _recv_statu = RECV_HTTP_BODY;
            // std::cout << "头部解析完" << std::endl;
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
            //std::cout << "content_length:" << std::endl;
            if(_recv_statu != RECV_HTTP_BODY) return false;
            //1. 获取正文长度
            size_t content_length = _request.ContentLength();
            //std::cout << "content_length:" << content_length << std::endl;
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
                case RECV_HTTP_LINE: RecvHttpLine(buf);//printf("首行解析完\n");
                case RECV_HTTP_HEAD: RecvHttpHead(buf); //printf("头部接受完\n");
                case RECV_HTTP_BODY: RecvHttpBody(buf);//printf("正文接受完\n");
            }
            return;
        }
};

class HttpServer
{
    private:
        using Handler = std::function<void(const HttpRequest&, HttpResponse *)>;
        using Handlers = std::vector<std::pair<std::regex, Handler>>;
        Handlers _get_route;
        Handlers _post_route;
        Handlers _put_route;
        Handlers _delete_route;
        std::string _basedir;   //静态资源根目录
        TcpServer _server;

    private:
        //填充一个错误显示页面数据到rsp中
        void ErrorHandler(const HttpRequest &req, HttpResponse *rsp)
        {
            //1. 组织一个错误展示页面
            std::string body;
            body += "<html>";
            body += "<head>";
            body += "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'>";
            body += "</head>";
            body += "<body>";
            body += "<h1>";
            body += std::to_string(rsp->_statu);
            body += " ";
            body += Util::StatuDesc(rsp->_statu);
             body += "</h1>";
            body += "</body>";
            body += "</html>";
            //2. 将页面数据，当作响应正文，放入rsp中
            rsp->SetContent(body, "text/html");
        }

        //将HttpResponse中的要素按照http协议格式进行组织，发送
        void WriteResponse(const PtrConnection &conn, const HttpRequest &req, HttpResponse &rsp)
        {
            //1. 先完善头部字段
            if(req.Close() == true)
            {
                rsp.SetHeader("Connection", "close");
            }else{
                rsp.SetHeader("Connection", "keep-alive");
            }
            if(rsp._body.empty() == false && rsp.HasHeader("Content-Length") == false)
            {
                rsp.SetHeader("Content-Length", std::to_string(rsp._body.size()));
            }
            if(rsp._body.empty() == false && rsp.HasHeader("Content-Type") == false)
            {
                rsp.SetHeader("Content-Type", "application/octet-stream");
            }
            if(rsp._redirect_flag == true)
            {
                rsp.SetHeader("Location", rsp._redirect_url);
            }
            //2. 将rsp中的要素，按照http协议格式进行组织
            std::stringstream rsp_str;
            rsp_str << req._version << " " << std::to_string(rsp._statu) << " " << Util::StatuDesc(rsp._statu) << "\r\n";
            for(auto &head : rsp._headers)
            {
                rsp_str << head.first << ": " << head.second << "\r\n";
            }
            rsp_str << "\r\n";
            rsp_str << rsp._body;
            //3. 发送数据
            conn->Send(rsp_str.str().c_str(), rsp_str.str().size());
        }

        //判断是否请求静态资源
        bool IsFileHandler(const HttpRequest &req)
        {
            // 1. 必须设置了静态资源根目录
            if(_basedir.empty())
            {
                return false;
            }
            // 2. 请求方法，必须是GET / HEAD请求方法
            if(req._method != "GET" && req._method != "HEAD")
            {
                return false;
            }
            // 3. 请求的资源路径必须是一个合法路径
            if(Util::ValidPath(req._path) == false)
            {
                return false;
            }
            // 4. 请求的资源必须存在,且是一个普通文件
            //    有一种请求比较特殊 -- 目录：/, /image/， 这种情况给后边默认追加一个 index.html
            // index.html    /image/a.png
            // 不要忘了前缀的相对根目录,也就是将请求路径转换为实际存在的路径  /image/a.png  ->   ./wwwroot/image/a.png
            std::string req_path = _basedir + req._path;//为了避免直接修改请求的资源路径，因此定义一个临时对象
            if(req._path.back() == '/')
            {
                req_path += "index.html";
            }
            if(Util::IsRegular(req_path) == false)//检查指定的路径是否指向一个普通的文件
            {
                return false;
            }
            return true;
        }
        //静态资源的请求处理 --- 将静态资源文件的数据读取出来，放到rsp的_body中, 并设置mime
        void FileHandler(const HttpRequest &req, HttpResponse *rsp)
        {
            std::string req_path = _basedir + req._path;//为了避免直接修改请求的资源路径，因此定义一个临时对象
            if(req._path.back() == '/')
            {
                req_path += "index.html";
            }
            bool ret = Util::ReadFile(req_path, &rsp->_body);
            if(ret == false)
            {
                return;
            }
            std::string mime = Util::ExtMime(req_path);
            rsp->SetHeader("Content-Type", mime);
            return;
        }

        //功能性请求的分类处理
        void Dispatcher(HttpRequest &req, HttpResponse *rsp, Handlers &handlers)
        {
            //在对应请求方法的路由表中，查找是否含有对应资源请求的处理函数，有则调用，没有则发出404
            //思想：路由表存储的是键值对 -- 正则表达式 & 处理函数
            //使用正则表达式，对请求的资源路径进行正则匹配，匹配成功就使用对应函数进行处理
            //例如：  /numbers/(\d+)   后者与前者匹配    /numbers/12345
            for(auto &handler : handlers)
            {
                const std::regex &re = handler.first;
                const Handler &functor = handler.second;
                bool ret = std::regex_match(req._path, req._matches, re);
                if(ret == false){continue;}
                return functor(req, rsp);//ret == true, 传入请求信息，和空的rsp，执行处理函数
            }
            rsp->_statu = 404;//无可调用
        }

        //对请求进行分辨
        void Route(HttpRequest &req, HttpResponse *rsp)
        {
            //1. 对请求进行分辨，是一个静态资源请求，还是一个功能性请求
            //   静态资源请求，则进行静态资源的处理
            //   功能性请求，则需要通过几个请求路由表来确定是否有处理函数
            //   既不是静态资源请求，也没有设置对应的功能性请求处理函数，就返回405
            if(IsFileHandler(req) == true)
            {
                //是一个静态资源请求, 则进行静态资源请求的处理   
                return FileHandler(req, rsp);
            }
            if(req._method == "GET" || req._method == "HEAD")
            {
                return Dispatcher(req, rsp, _get_route);
            }
            else if(req._method == "POST")
            {
                return Dispatcher(req, rsp, _post_route);
            }
            else if(req._method == "PUT")
            {
                return Dispatcher(req, rsp, _put_route);
            }  
            else if(req._method == "DELETE")
            {
                return Dispatcher(req, rsp, _delete_route);
            }
            rsp->_statu = 405;// Method Not Allowed
            return;
        }

        //设置上下文,连接建立后的回调
        void OnConnected(const PtrConnection &conn)
        {
            conn->SetContext(HttpContext());//初始化了上下文
            DBG_LOG("NEW CONNECTION %p", conn.get());
        }

        //缓冲区数据解析+处理   消息回调函数
        void OnMessage(const PtrConnection &conn, Buffer *buf)
        {
            //std::cout << "OnMessage" << std::endl;
            while (buf->ReadAbleSize() > 0)
            {
                //1. 获取上下文
                HttpContext *context = conn->GetContext()->get<HttpContext>();
                
                //2. 通过上下文对缓冲区数据进行解析，得到HttpRequest对象
                //  a. 如果缓冲区的数据解析出错，就直接回复出错响应
                //  b. 如果解析正常，且请求已经获取完毕，才开始去进行处理
                context->RecvHttpRequest(buf);
                HttpRequest &req = context->Request();
                HttpResponse rsp(context->RespStatu());//用当前上下文的_resp_statu初始化一个rsp
                if(context->RespStatu() >= 400)
                {
                    //进行错误响应，关闭连接
                    ErrorHandler(req, &rsp);//填充一个错误显示页面数据到rsp中
                    WriteResponse(conn, req, rsp);//组织响应发送给客户端
                    context->ReSet();
                    buf->MoveReadOffset(buf->ReadAbleSize());//出错了就把缓冲区数据清空
                    conn->Shutdown();//关闭连接
                    return;
                }
                if(context->RecvStatu() != RECV_HTTP_OVER)
                {
                    //当前请求还没有接收完整,则退出，等新数据到来再重新继续处理
                    return;
                }
                //3. 请求路由 + 业务处理
                Route(req, &rsp);
                //4. 对HttpResponse进行组织发送
                WriteResponse(conn, req, rsp);
                //5. 重置上下文
                context->ReSet();
                //6. 根据长短连接判断是否关闭连接或者继续处理
                if(rsp.Close() == true) conn->Shutdown();//短链接则直接关闭
            }
            return;      
        }

    public:
        HttpServer(int port, int timeout = DEFALT_TIMEOUT):_server(port)
        {
            _server.EnableInactiveRelease(timeout);
            _server.SetConnectedCallback(std::bind(&HttpServer::OnConnected, this, std::placeholders::_1));
            _server.SetMessageCallback(std::bind(&HttpServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
        }

        void SetBaseDir(const std::string &path)
        {
            assert(Util::IsDirectory(path) == true);
            _basedir = path;
        }

        //  设置/添加，请求（请求的正则表达）与处理函数的映射关系
        void Get(const std::string &pattern, const Handler &handler)
        {
            _get_route.push_back(std::make_pair(std::regex(pattern), handler));
        }
        void Post(const std::string &pattern, const Handler &handler)
        {
            _post_route.push_back(std::make_pair(std::regex(pattern), handler));
        }
        void Put(const std::string &pattern, const Handler &handler)
        {
            _put_route.push_back(std::make_pair(std::regex(pattern), handler));
        }
        void Delete(const std::string &pattern, const Handler &handler)
        {
            _delete_route.push_back(std::make_pair(std::regex(pattern), handler));
        }

        void SetThreadCount(int count)
        {
            _server.SetThreadCount(count);
        }
        void Listen()
        {
            _server.Start();
        }
};