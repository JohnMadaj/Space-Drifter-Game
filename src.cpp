#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
 #include <SDL2/SDL_ttf.h>
#include <string>
#include <cmath>
#include <iostream>
#include <chrono>

const int SCREEN_WIDTH =  2600;
const int SCREEN_HEIGHT = 1500;
const int framerate = 60; //60 is ideal for thruster animation

int ship_speed = 600;
int ship_turn_speed = 4;
float accel_constant = 0.3; // m/s^2

float SHIP_SIZE = 500;

const float dtor = 3.14159265/180;//1/565.4866776461628; //degrees-to-radians

int moveShip(float up, float down, float *velocityX, float *velocityY,
		double degrees, SDL_Rect *map) {
	if (up) {

		*velocityX -= accel_constant * up
				+ (-(ship_speed / 30) * sin(degrees * dtor));
		*velocityY += accel_constant * up
				+ ((ship_speed / 30) * cos(degrees * dtor));
	}
	if (down) {
		*velocityX += accel_constant * down
				+ (-(ship_speed / 30) * sin(degrees * dtor));
		*velocityY -= accel_constant * down
				+ ((ship_speed / 30) * cos(degrees * dtor));
	}
	return 0;
}

int adjustShip(float *velocityX, float *velocityY, double degrees,
		SDL_Rect *map) {

	map->x += (-*velocityX);
	map->y += (*velocityY);

	return 0;
}

int main(int argc, char *argv[])
{
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("error initializing SDL: %s\n", SDL_GetError());
    if (TTF_Init() != 0) std::cout<<("ERROR INITIALIING TTF");

    SDL_Window* win = SDL_CreateWindow("Space Drifter by John Madaj", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    // triggers the program that controls graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    // creates a renderer to render our images
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    // Load our font file and set the font size

    TTF_Font* ourFont = TTF_OpenFont("./res/Roboto-Regular.ttf",32);
    // Confirm that it was loaded
    if(ourFont == nullptr){
    	std::cout << "Could not load font" << std::endl;
        exit(1);
    }

    //image path to surface
    SDL_Surface* ship_surface = IMG_Load("res/ship.png");
    SDL_Surface* fship_surface = IMG_Load("res/ship_flying.png");
//    SDL_Surface* bship_surface = IMG_Load("res/ship_blue.png");
    SDL_Surface* bg = IMG_Load("res/space_background.png");

    SDL_Surface* surfaceText = TTF_RenderText_Solid(ourFont,"FPS:",{255,255,255});


    // loads image to graphics hardware memory.
    SDL_Texture* ship = SDL_CreateTextureFromSurface(rend, ship_surface);
    SDL_Texture* space_background = SDL_CreateTextureFromSurface(rend, bg);
    SDL_Texture* fship = SDL_CreateTextureFromSurface(rend, fship_surface);
//    SDL_Texture* bship = SDL_CreateTextureFromSurface(rend, bship_surface);

    SDL_Texture* textureText = SDL_CreateTextureFromSurface(rend,surfaceText);

    Uint32 colorkey = SDL_MapRGB(bg->format, 0, 0, 0);
    SDL_SetColorKey(bg, SDL_TRUE, colorkey);

    // clears main-memory
    SDL_FreeSurface(ship_surface);
    SDL_FreeSurface(fship_surface);
    SDL_FreeSurface(bg);
//    SDL_FreeSurface(bship_surface);
    SDL_FreeSurface(surfaceText);


    // Create rectangles to draw on
	SDL_Rect rectText;
				rectText.x = 10;
				rectText.y = 10;
				rectText.w = 100;
				rectText.h = 50;

	SDL_Rect rectText2;
				rectText2.x = 130;
				rectText2.y = 10;
				rectText2.w = 200;
				rectText2.h = 50;

    //set static rectangle for ship and adjust size/position
	SDL_Rect rectShip;
				float SHIP_RATIO = 0.41009592;
				rectShip.w = SHIP_SIZE * SHIP_RATIO;    ///= 6;
				rectShip.h = SHIP_SIZE;    ///= 6;
				// sets initial x-position of object
				rectShip.x = 1140;
				// sets initial y-position of object
				rectShip.y = 600;		//(1000 - map.h) / 2;

	SDL_Rect map;
				// connects our texture with map to control position
				SDL_QueryTexture(space_background, NULL, NULL, &map.w, &map.h);

				//adjust height and width of our image box. COMMENT THESE OUT
				//map.w /= 6;
				//map.h /= 6;

				// sets initial x-position of object
				map.x = (1000 - map.w) / 2;

				// sets initial y-position of object
				map.y = (1000 - map.h) / 2;

    //environment RenderEx values
	double degrees = 0;
	SDL_RendererFlip flipType = SDL_FLIP_NONE;
	SDL_Point *center = NULL;

    //track duration of inputs, increase as long as button is held down
    float up = 0;
    float down = 0;
    float left = 0;
    float right = 0;

    float velocityX = 0;
    float velocityY = 0;

    // entering game loop
    int close = 0;
    while (!close) {	//animation loop
        SDL_Event event;
        //Events management
        while (SDL_PollEvent(&event)) {

        	const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);

        	//MOVEMENT ON KEYDOWN
			if (event.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_UP]) up += 0.01;

			if (event.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_DOWN]) down += 0.01;

			if (event.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_LEFT]) left += 0.01;

			if (event.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_RIGHT]) right += 0.01;

			//Space bar input slows the ship down
			if (event.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_SPACE]){
				velocityX /= 1.1;
				velocityY /= 1.1;
			}

			//MOVEMENT ON KEYUP
			if (event.type == SDL_KEYUP &&!keyboard_state_array[SDL_SCANCODE_UP]) up = 0;

			if (event.type == SDL_KEYUP &&!keyboard_state_array[SDL_SCANCODE_DOWN]) down = 0;

			if (event.type == SDL_KEYUP &&!keyboard_state_array[SDL_SCANCODE_LEFT]) left = 0;

			if (event.type == SDL_KEYUP &&!keyboard_state_array[SDL_SCANCODE_RIGHT]) right = 0;

        	moveShip(up, down, &velocityX, &velocityY, degrees, &map);
        	if (left) degrees -= ship_turn_speed;
        	if (right) degrees += ship_turn_speed;

        	//UPDATE RENDER
        	SDL_RenderCopyEx(rend, fship, NULL, &rectShip, degrees, center, flipType);
        	SDL_RenderPresent(rend);

//        	std::cout<<velocityX<<"\t"<<velocityY<<std::endl;
        }

		adjustShip(&velocityX, &velocityY, degrees, &map);
		SDL_RenderCopyEx(rend, ship, NULL, &rectShip, degrees, center, flipType);
		SDL_RenderPresent(rend);

		// right boundary
		if (map.x < -7500) {
			map.x = -7500;
			velocityX = 0;
		}
//
		// left boundary
		if (map.x > 2000) {
			map.x = 2000;
			velocityX = 0;
		}

		// upper boundary
		if (map.y > 500) {
			map.y = 500;
			velocityY = 0;
		}
//
		// lower boundary
		if (map.y < -7000) {
			map.y = -7000;
			velocityY = 0;
		}

        // handling of close button
        if (event.type==SDL_QUIT) {
            close = 1;}

        auto p1 = std::chrono::system_clock::now();

        // clears the screen
        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, space_background, NULL, &map);
        SDL_RenderCopyEx(rend, ship, NULL, &rectShip, degrees,
        		center, flipType);
        SDL_RenderCopy(rend,textureText,NULL, &rectText);

//        SDL_RenderSetScale(rend, 0.5, 0.5);

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(rend);

        // calculates fps
        SDL_Delay(1000 / framerate);
        auto p2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = p2 - p1;

//        std::cout<<map.x<<map.y<<std::endl;


        //this parts annoying
        SDL_Surface* Snewtext = TTF_RenderText_Solid(ourFont, std::to_string(1/elapsedTime.count()).c_str() ,{255,255,255});
        SDL_Texture* Tnewtext = SDL_CreateTextureFromSurface(rend,Snewtext);
        SDL_RenderCopy(rend,Tnewtext,NULL,&rectText2);
        SDL_FreeSurface(Snewtext);
        SDL_DestroyTexture(Tnewtext);
    }


    // Destroy texture
    SDL_DestroyTexture(ship);
    SDL_DestroyTexture(space_background);
    SDL_DestroyTexture(textureText);

    TTF_CloseFont(ourFont);

    // Destroy renderer
    SDL_DestroyRenderer(rend);

    // Destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return 0;
}
