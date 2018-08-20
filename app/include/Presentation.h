#ifndef DEF_HEADER_PRESENTATION
#define DEF_HEADER_PRESENTATION

class Presentation {
public:
	Presentation(Scene * scene, void (*actions)(Scene *), Presentation * next = NULL);
	~Presentation();
	void run();
	Presentation * next_frame();
private:
	Scene * current_scene;
	void (*actions)(Scene *);
	Presentation * next;
};

#endif