#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <omp.h>
#include "iostream"
#include <sys/time.h>
double timer()
{
	struct timeval ts;
	gettimeofday(&ts, NULL);
	return (double)ts.tv_sec + 1e-6 * (double)ts.tv_usec;
}


int main(int argc, char const *argv[])
{
    // Create main window
    if(argc != 4)
        return -1;
    int height = std::stoi(argv[1]);
    int width = std::stoi(argv[2]);
    int num_of_threads = std::stoi(argv[3]);
    // omp_set_num_threads(num_of_threads); 
    sf::RenderWindow App(sf::VideoMode(height, width), "SFML Graphics");
	
	sf::Image buffer;
    sf::Image buffer_prev;
    buffer.create(height, width, sf::Color(0, 0, 0));
	buffer_prev.create(height, width, sf::Color(0, 0, 0));

    sf::Texture texture;
    texture.loadFromImage(buffer);
	sf::Sprite bufferSprite(texture);
	
	// Random seed
	srand(11);
	
	int randX, randY;
	int r, g, b;
	
	App.clear();

    for(auto i = 0; i < height; i++){
            for(auto j = 0; j < width; j++){
                // std::cout<<rand()<<std::endl;
                sf::Color color = rand() % 5 == 0? sf::Color::Red : sf::Color::Black;
                buffer.setPixel(i,j, color);
            }
    } 
    int counter = 0;
    double t = timer();
    while (App.isOpen())
    {
        sf::Event Event;
        while (App.pollEvent(Event))
        {
            if (Event.type == sf::Event::Closed)
                App.close();
        }

        

		// buffer.setPixel(0,0, sf::Color::Green);
		// buffer.setPixel(randX, randY, sf::Color(r, g, b));
#pragma omp parallel for default(shared) num_threads(num_of_threads)
        for(auto i = 0; i < height; i++){
            for(auto j = 0; j < width; j++){
                if(buffer_prev.getPixel(i,j) == sf::Color::Black){
                    int count = 0;
                    for(auto k = -1; k <= 1; k++){
                        for(auto l = -1; l <= 1; l++){
                            if(buffer_prev.getPixel((height + (i + k)) % height, (width + (j + l)) % width) == sf::Color::Red)
                                count++;
                            // if(i== 0)
                            //     std::cout<<(width + (j + l)) % width<<std::endl;
                                             
                        }
                    }
                    if(count == 3)
                        buffer.setPixel(i,j, sf::Color::Red);
                }
                if(buffer_prev.getPixel(i,j) == sf::Color::Red){
                    int countR = 0;
                    for(auto k = -1; k <= 1; k++){
                        for(auto l = -1; l <= 1; l++){
                            if(buffer_prev.getPixel((height + (i + k)) % height,  (width + (j + l)) % width) == sf::Color::Red)
                                countR++;
                        }
                    }

                    if(countR == 4 || countR == 3)
                        buffer.setPixel(i,j, sf::Color::Red);
                    else
                        buffer.setPixel(i,j, sf::Color::Black);
                }
            }
        } 
		texture.update(buffer);
		App.draw(bufferSprite);
        App.display();
        buffer_prev = buffer;
        counter++;
        if(counter == 100){
            std::cout<<timer() - t<<std::endl;
            t = timer();
            counter = 0;
        }
    }

    return 0;
}