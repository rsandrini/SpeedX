#ifndef ROTATINGTRIANGLE_H_INCLUDED
#define ROTATINGTRIANGLE_H_INCLUDED
#include "GameSteps.h"
#include "tVector.h"

class Game : public GameSteps
{
    private:
        bool exit;

    public:
		explicit Game() : exit(false) {}

        virtual void setup();

        virtual void processEvents(const SDL_Event& event);
        virtual void processLogics(float secs);
        virtual void draw() const;
		virtual bool ended() { return exit; }
        virtual void teardown();
		void drawTunnel() const;
		void generateMap() const;
		void rebuildMap() const;
		void realocPosition(int position) const;
		void generateOneRing() const;
		void drawQuad() const;
};

#endif // ROTATINGTRIANGLE_H_INCLUDED
