#include"morphing.h"

int main() {
	CImg<unsigned char> source,target;
	source.load_bmp("1.bmp");
	target.load_bmp("3.bmp");
	auto mh = morphing();
	mh.setSource(source);
	mh.setTarget(target);
	//source.savevid
	auto imgs = mh.morph(11);
	for (int i = 0; i < imgs.size(); i++) {
		auto img = imgs[i];
		//img.save_vide
		string name = "result";
		char s[12];
		_itoa_s(i, s, 10);
		name.append(s);
		name.append(".bmp");
		
		img.save_bmp(name.c_str());
		img.display();
		//img.display
	}

	cout << "imgs nums : " << imgs.size() << endl;
	//mh.show_source();
	
	mh.pause();
}
