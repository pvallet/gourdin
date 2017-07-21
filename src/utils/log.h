#include <sstream>

/** Singleton class in which any function can store info to be displayed on the
  * screen
	*/
class Log	{
public:
	static Log& getInstance() {
    static Log instance;
    return instance;
  }

	Log(Log const&)             = delete;
	void operator=(Log const&)  = delete;

	inline void clear() {_text.str("");}
	inline void addLine(std::string newLine) { _text << newLine;}
	void addFPSandCamInfo(int msElapsed);
	inline std::string getText() const {return _text.str();}


private:
	Log() {}

	std::ostringstream _text;
};
