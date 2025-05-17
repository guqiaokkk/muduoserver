#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <sys/stat.h>
#include "../server.hpp"

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
