#ifndef NUITRACK_COLOR3_H_
#define NUITRACK_COLOR3_H_

namespace tdv
{
namespace nuitrack
{

struct Color3
{
	Color3(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
		: red(red), green(green), blue(blue) {}

	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

}
}

#endif /* NUITRACK_COLOR3_H_ */
