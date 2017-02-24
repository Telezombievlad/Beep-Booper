#ifndef MY_SDL_RENDERER_HPP_INCLUDED
#define MY_SDL_RENDERER_HPP_INCLUDED

//----------------------------------------------------------------------------
//{ Includes
//----------------------------------------------------------------------------

    #include <iostream>
    #include <iomanip>
    #include <functional>
    #include <cstdlib>
    #include <random>
    
    #include <SDL2/SDL.h>

//}
//----------------------------------------------------------------------------


namespace MySDL
{

//----------------------------------------------------------------------------
//{ Renderer
//----------------------------------------------------------------------------

    class Renderer
    {
        public:

            // Constructor && destructor:

                Renderer(SDL_Window* window);

                ~Renderer();

            // Getters && setters:

                size_t getDestSizeX() const;
                size_t getDestSizeY() const;

            // Functions:

                // Debugging:

                    void dump() const;

                // Additional:

                    void flash(const SDL_Rect* src = nullptr, const SDL_Rect* dest = nullptr) const;

                    Renderer&  startRendering();
                    Renderer& finishRendering();

                // Colors:

                    Renderer& setLineColor(const SDL_Color& lineColor);
                    Renderer& setFillColor(const SDL_Color& fillColor);

                // Rendering:

                    Renderer& clear(const SDL_Color& color);
                    Renderer& clear();

                    Renderer& pixel(const int x, const int y, const SDL_Color& color);
                    Renderer& pixel(const int x, const int y);

                    Renderer& line(int x0, int y0, int x1, int y1, const SDL_Color& color);
                    Renderer& line(int x0, int y0, int x1, int y1);

                    Renderer& circle(const int x, const int y, const unsigned int r, const SDL_Color& color);
                    Renderer& circle(const int x, const int y, const unsigned int r);

                    Renderer& round(const int x, const int y, const unsigned int r, const SDL_Color& lineColor, const SDL_Color& fillColor);
                    Renderer& round(const int x, const int y, const unsigned int r);

                    Renderer& applyShader
                    (
                        const int x, 
                        const int y, 
                        unsigned int w, 
                        unsigned int h, 
                        std::function<SDL_Color(int x, int y, const SDL_Color& color)> shader
                    );

        private:

            // Variables:

                SDL_Renderer* renderer_;
                SDL_Texture*  dest_;

                unsigned int destSizeX_;
                unsigned int destSizeY_;
                
                Uint8* pixelBuffer_;

                SDL_Color lineColor_;
                SDL_Color fillColor_;

            // Functions, that shouldn't appear anywhere at all:

                Renderer();

                Renderer(const Renderer& renderer);
                
                Renderer& operator=(const Renderer& renderer);

            // Functions, that should not appear anywhere outside: 

                Renderer& clearInsecure(const SDL_Color& color);

                Renderer& pixelInsecure(const int x, const int y, const SDL_Color& color);

                Renderer& lineInsecure(int x0, int y0, int x1, int y1, const SDL_Color& color);

                Renderer& circleInsecure(const int x, const int y, const unsigned int r, const SDL_Color& color);

                Renderer& roundInsecure(const int x, const int y, const unsigned int r, const SDL_Color& lineColor, const SDL_Color& fillColor);

                Renderer& applyShaderInsecure
                (
                    const int x, 
                    const int y, 
                    const unsigned int w, 
                    const unsigned int h, 
                    std::function<SDL_Color(int x, int y, const SDL_Color& color)> shader
                );
    };


    //----------------------------------------------------------------------------
    //{ Constructor && destructor
    //----------------------------------------------------------------------------

        Renderer::Renderer(SDL_Window* window) :
            renderer_    (nullptr),
            dest_        (nullptr), 
            destSizeX_   (0),
            destSizeY_   (0),
            pixelBuffer_ (nullptr),
            lineColor_   ({0, 0, 0, 255}),
            fillColor_   ({0, 0, 0, 255})
        {
            if (window == nullptr)
            {
                throw std::invalid_argument("Renderer::constructor: (SDL_Window*) window is a null pointer\n");
            }

            int windowSizeX = 0, windowSizeY = 0;
            SDL_GetWindowSize(window, &windowSizeX, &windowSizeY);
            if (windowSizeX <= 0)
            {
                throw std::string("Renderer::constructor: window width is negative or 0\n") + std::string(SDL_GetError());
            }
            if (windowSizeY <= 0)
            {
                throw std::string("Renderer::constructor: window height is negative or 0\n") + std::string(SDL_GetError());
            }
            destSizeX_ = windowSizeX;
            destSizeY_ = windowSizeY;

            renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer_ == nullptr)
            {   
                throw std::string("Renderer::constructor: SDL_CreateRenderer failed\n") + std::string(SDL_GetError());                 
            }

            dest_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, destSizeX_, destSizeY_);
            if (dest_ == nullptr)
            {
                SDL_DestroyRenderer(renderer_);

                throw std::string("Renderer::constructor: SDL_CreateTexture failed\n") + std::string(SDL_GetError()); 
            }

            // If we're here, the invariant is set, no assert(ok()) needed then, right?
        }

        Renderer::~Renderer()
        {
            SDL_DestroyTexture(dest_);

            SDL_DestroyRenderer(renderer_);
        }

    //}
    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------
    //{ Getters && setters:
    //----------------------------------------------------------------------------

        size_t Renderer::getDestSizeX() const
        {
            return destSizeX_;
        }

        size_t Renderer::getDestSizeY() const
        {
            return destSizeY_;
        } 

    //}
    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------
    //{ Functions
    //----------------------------------------------------------------------------

        //----------------------------------------------------------------------------
        //{ Debugging
        //----------------------------------------------------------------------------

            #ifndef NDEBUG

                void Renderer::dump() const
                {
                    std::cout << "\nRenderer::dump:"                << "\n"
                              << "renderer_    == " << renderer_    << "\n"
                              << "dest_        == " << dest_        << "\n"
                              << "destSizeX_   == " << destSizeX_   << "\n"
                              << "destSizeY_   == " << destSizeY_   << "\n"
                              << "pixelBuffer_ == " << static_cast<void*>(pixelBuffer_) << "\n"
                              << std::setfill('0') << std::right 
                              << "lineColor_   ==" 
                              << " r" << std::setw(3) << static_cast<int>(lineColor_.r)
                              << " g" << std::setw(3) << static_cast<int>(lineColor_.g)
                              << " b" << std::setw(3) << static_cast<int>(lineColor_.b)
                              << " a" << std::setw(3) << static_cast<int>(lineColor_.a) << "\n"
                              << "fillColor_   ==" 
                              << " r" << std::setw(3) << static_cast<int>(fillColor_.r)
                              << " g" << std::setw(3) << static_cast<int>(fillColor_.g)
                              << " b" << std::setw(3) << static_cast<int>(fillColor_.b)
                              << " a" << std::setw(3) << static_cast<int>(fillColor_.a) << "\n"
                              << std::setfill(' ') << std::left
                              <<std::endl;
                }

            #else 

                void Renderer::dump() const {}

            #endif /*NDEBUG*/

        //}
        //----------------------------------------------------------------------------


        //----------------------------------------------------------------------------
        //{ Additional
        //----------------------------------------------------------------------------

            void Renderer::flash(const SDL_Rect* src /*= nullptr*/, const SDL_Rect* dest /*= nullptr*/) const
            {
                if (pixelBuffer_ != nullptr)
                {
                   throw std::string("Renderer::render(): rendering is not finished (texture is still locked)");
                }

                if (SDL_RenderCopy(renderer_, dest_, src, dest) != 0)
                {
                    throw std::string("Renderer::render(): SDL_RenderCopy failed.\n") + std::string(SDL_GetError());
                }

                SDL_RenderPresent(renderer_);
            }

            Renderer& Renderer::startRendering()
            {
                if (pixelBuffer_ != nullptr)
                {
                    throw std::string("Renderer::startRendering(): rendering is already in process (texture is already locked)");  
                } 

                int pitch = 0;
                if (SDL_LockTexture(dest_, nullptr, reinterpret_cast<void**>(&pixelBuffer_), &pitch) != 0)
                {
                    throw std::string("Renderer::startRendering(): SDL_LockTexture failed\n") + std::string(SDL_GetError());
                }

                destSizeX_ = pitch/4;

                return *this;
            }

            Renderer& Renderer::finishRendering()
            {
                if (pixelBuffer_ == nullptr)
                {
                    throw std::string("Renderer::finishRendering(): rendering has not been started yet (texture is not locked)"); 
                } 

                SDL_UnlockTexture(dest_);

                pixelBuffer_ = nullptr;

                return *this;
            }

        //}
        //----------------------------------------------------------------------------


        //----------------------------------------------------------------------------
        //{ Colors
        //----------------------------------------------------------------------------

            Renderer& Renderer::setLineColor(const SDL_Color& lineColor)
            {
                lineColor_ = lineColor;

                return *this;
            }

            Renderer& Renderer::setFillColor(const SDL_Color& fillColor)
            {
                fillColor_ = fillColor;

                return *this;
            }

        //}
        //----------------------------------------------------------------------------


        //----------------------------------------------------------------------------
        //{ Rendering
        //----------------------------------------------------------------------------

            //----------------------------------------------------------------------------
            //{ Clearing
            //----------------------------------------------------------------------------

                Renderer& Renderer::clearInsecure(const SDL_Color& color)
                {
                    for (size_t index = 0; index < 4 * destSizeX_ * destSizeY_; index += 4)
                    {
                        pixelBuffer_[index + 0] = color.r;
                        pixelBuffer_[index + 1] = color.g;
                        pixelBuffer_[index + 2] = color.b;
                        pixelBuffer_[index + 3] = color.a;
                    }

                    return *this;
                }

                Renderer& Renderer::clear(const SDL_Color& color)
                {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::clear(): rendering has not been started yet (texture is not locked)"); 
                    }

                    return clearInsecure(color);
                }

                Renderer& Renderer::clear()
                {
                    return clear(fillColor_);
                }
                
            //}
            //----------------------------------------------------------------------------


            //----------------------------------------------------------------------------
            //{ Pixel
            //----------------------------------------------------------------------------

                Renderer& Renderer::pixelInsecure(const int x, const int y, const SDL_Color& color)
                {
                    size_t index = (destSizeX_ * y + x) * 4;
                    
                    pixelBuffer_[index + 0] = color.r;
                    pixelBuffer_[index + 1] = color.g;
                    pixelBuffer_[index + 2] = color.b;
                    pixelBuffer_[index + 3] = color.a;

                    return *this;
                }

                Renderer& Renderer::pixel(const int x, const int y, const SDL_Color& color)
                {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::pixel(): rendering has not been started yet (texture is not locked)"); 
                    }

                    if (0 > x || x >= static_cast<int>(destSizeX_))
                    {
                        return *this;
                    }

                    if (0 > y || y >= static_cast<int>(destSizeY_))
                    {
                        return *this;
                    }
                    
                    return pixelInsecure(x, y, color);
                }

                Renderer& Renderer::pixel(const int x, const int y)
                {
                    return pixel(x, y, lineColor_);
                }

            //}
            //----------------------------------------------------------------------------


            //----------------------------------------------------------------------------
            //{ Pixel
            //----------------------------------------------------------------------------

                Renderer& Renderer::lineInsecure(int x0, int y0, int x1, int y1, const SDL_Color& color)
                {
                    bool swappedXandY = false;

                    if (abs(y1 - y0) > abs(x1 - x0))
                    {
                        std::swap(y0, x0);
                        std::swap(y1, x1);

                        swappedXandY = true;
                    }

                    if (x1 < x0)
                    {
                        std::swap(x0, x1);
                        std::swap(y0, y1);
                    }

                    int dX = x1 - x0;
                    int dY = y1 - y0;
                    int dX2 = dX << 1;

                    int error2dX = 0;
                    int deltaError = -(dY << 1);

                    for (int x = x0, y = y0; x <= x1; x++, error2dX += deltaError)
                    {
                        if (swappedXandY) pixelInsecure(y, x, color);
                        else pixelInsecure(x, y, color);

                        if (error2dX < -dX)
                        {
                            error2dX += dX2;
                            y++;
                        }
                        else if (error2dX > dX)
                        {
                            error2dX -= dX2;
                            y--;
                        }
                    }

                    return *this;
                }

                Renderer& Renderer::line(int x0, int y0, int x1, int y1, const SDL_Color& color)
                {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::line(): rendering has not been started yet (texture is not locked)"); 
                    }

                    SDL_Rect screen = {0, 0, static_cast<int>(destSizeX_ - 1), static_cast<int>(destSizeY_ - 1)};

                    SDL_IntersectRectAndLine(&screen, &x0, &y0, &x1, &y1);

                    if (x0 < 0 || x0 >= static_cast<int>(destSizeX_) || y0 < 0 || y0 >= static_cast<int>(destSizeY_)) return *this;

                    return lineInsecure(x0, y0, x1, y1, color);
                }

                Renderer& Renderer::line(int x0, int y0, int x1, int y1)
                {
                    return line(x0, y0, x1, y1, lineColor_);
                }
            
            //}
            //----------------------------------------------------------------------------


            //----------------------------------------------------------------------------
            //{ Circle
            //----------------------------------------------------------------------------

                 Renderer& Renderer::circleInsecure(const int x, const int y, const unsigned int r, const SDL_Color& color)
                 {
                    int error = 0;                    

                    for (int relX = -r, relY = 0; relY < -relX; ++relY, error += 2 * relY + 1)
                    {
                        if (std::labs(error + 2 * relX + 1) < std::labs(error))
                        {
                            ++relX;

                            error += 2 * relX + 1;
                        }

                        pixelInsecure(x + relX, y + relY, color);
                        pixelInsecure(x + relX, y - relY, color);
                        pixelInsecure(x - relX, y + relY, color);
                        pixelInsecure(x - relX, y - relY, color);
                        pixelInsecure(x + relY, y + relX, color);
                        pixelInsecure(x + relY, y - relX, color);
                        pixelInsecure(x - relY, y + relX, color);
                        pixelInsecure(x - relY, y - relX, color);   
                    }

                    return *this;
                 }

                 Renderer& Renderer::circle(const int x, const int y, const unsigned int r, const SDL_Color& color)
                 {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::circle(): rendering has not been started yet (texture is not locked)"); 
                    }

                    int error = 0;                    

                    for (int relX = -r, relY = 0; relY < -relX; ++relY, error += 2 * relY + 1)
                    {
                        if (std::labs(error + 2 * relX + 1) < std::labs(error))
                        {
                            ++relX;

                            error += 2 * relX + 1;
                        }

                        pixel(x + relX, y + relY, color);
                        pixel(x + relX, y - relY, color);
                        pixel(x - relX, y + relY, color);
                        pixel(x - relX, y - relY, color);
                        pixel(x + relY, y + relX, color);
                        pixel(x + relY, y - relX, color);
                        pixel(x - relY, y + relX, color);
                        pixel(x - relY, y - relX, color);   
                    }

                    return *this;
                 }

                 Renderer& Renderer::circle(const int x, const int y, const unsigned int r)
                 {
                    return circle(x, y, r, lineColor_);
                 }
            
            //}
            //----------------------------------------------------------------------------


            //----------------------------------------------------------------------------
            //{ Round
            //----------------------------------------------------------------------------

                 Renderer& Renderer::roundInsecure(const int x, const int y, const unsigned int r, const SDL_Color& lineColor, const SDL_Color& fillColor)
                 {
                    int error = 0;                    

                    for (int relX = -r, relY = 0; relY < -relX; ++relY, error += 2 * relY + 1)
                    {
                        if (std::labs(error + 2 * relX + 1) < std::labs(error))
                        {
                            ++relX;

                            error += 2 * relX + 1;
                        }

                        pixelInsecure(x + relX, y + relY, lineColor);
                        pixelInsecure(x + relX, y - relY, lineColor);
                        pixelInsecure(x - relX, y + relY, lineColor);
                        pixelInsecure(x - relX, y - relY, lineColor);
                        pixelInsecure(x + relY, y + relX, lineColor);
                        pixelInsecure(x + relY, y - relX, lineColor);
                        pixelInsecure(x - relY, y + relX, lineColor);
                        pixelInsecure(x - relY, y - relX, lineColor); 

                        for (int curX = relX + 1; curX <= -relY; ++curX)
                        {
                            pixelInsecure(x + curX, y + relY, fillColor);
                            pixelInsecure(x + curX, y - relY, fillColor);
                            pixelInsecure(x - curX, y + relY, fillColor);
                            pixelInsecure(x - curX, y - relY, fillColor);
                            pixelInsecure(x + relY, y + curX, fillColor);
                            pixelInsecure(x + relY, y - curX, fillColor);
                            pixelInsecure(x - relY, y + curX, fillColor);
                            pixelInsecure(x - relY, y - curX, fillColor); 
                        }  
                    }

                    return *this;
                 }

                 Renderer& Renderer::round(const int x, const int y, const unsigned int r, const SDL_Color& color, const SDL_Color& fillColor)
                 {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::circle(): rendering has not been started yet (texture is not locked)"); 
                    }

                    int error = 0;                    

                    for (int relX = -r, relY = 0; relY < -relX; ++relY, error += 2 * relY + 1)
                    {
                        if (std::labs(error + 2 * relX + 1) < std::labs(error))
                        {
                            ++relX;

                            error += 2 * relX + 1;
                        }

                        pixel(x + relX, y + relY, color);
                        pixel(x + relX, y - relY, color);
                        pixel(x - relX, y + relY, color);
                        pixel(x - relX, y - relY, color);
                        pixel(x + relY, y + relX, color);
                        pixel(x + relY, y - relX, color);
                        pixel(x - relY, y + relX, color);
                        pixel(x - relY, y - relX, color); 

                        for (int curX = relX + 1; curX <= -relY; ++curX)
                        {
                            pixel(x + curX, y + relY, fillColor);
                            pixel(x + curX, y - relY, fillColor);
                            pixel(x - curX, y + relY, fillColor);
                            pixel(x - curX, y - relY, fillColor);
                            pixel(x + relY, y + curX, fillColor);
                            pixel(x + relY, y - curX, fillColor);
                            pixel(x - relY, y + curX, fillColor);
                            pixel(x - relY, y - curX, fillColor); 
                        }   
                    }

                    return *this;
                 }

                 Renderer& Renderer::round(const int x, const int y, const unsigned int r)
                 {
                    return round(x, y, r, lineColor_, fillColor_);
                 }

            //}
            //----------------------------------------------------------------------------


            //----------------------------------------------------------------------------
            //{ Apply function on a rectangle
            //----------------------------------------------------------------------------

                Renderer& Renderer::applyShaderInsecure
                (
                    const int x, 
                    const int y, 
                    const unsigned int w, 
                    const unsigned int h, 
                    std::function<SDL_Color(int x, int y, const SDL_Color& color)> shader
                )
                {
                    for (unsigned int curY = y; curY < y + h; ++curY)
                    {
                        size_t curIndex = (destSizeX_ * curY + x) * 4;

                        for (unsigned int curX = x; curX < x + w; ++curX, curIndex += 4)
                        {                        
                            SDL_Color curColor =
                            {
                                pixelBuffer_[curIndex + 0],
                                pixelBuffer_[curIndex + 1],
                                pixelBuffer_[curIndex + 2],
                                pixelBuffer_[curIndex + 3]
                            };

                            pixelInsecure(curX, curY, shader(curX, curY, curColor));
                        }
                    }

                    return *this;
                }

                Renderer& Renderer::applyShader(int x, int y, unsigned int w, unsigned int h, std::function<SDL_Color(int x, int y, const SDL_Color& color)> shader)
                {
                    if (pixelBuffer_ == nullptr)
                    {
                        throw std::string("Renderer::applyFunction(): rendering has not been started yet (texture is not locked)"); 
                    }

                    if (x >= static_cast<int>(destSizeX_) || y >= static_cast<int>(destSizeY_)) return *this;

                    if (x < 0) 
                    {   
                        if (-x > static_cast<int>(w)) return *this;
                        
                        w += x;
                        x = 0;
                    }

                    if (y < 0) 
                    {   
                        if (-y > static_cast<int>(h)) return *this;
                        
                        h += y;
                        y = 0;
                    }

                    if (x + w > destSizeX_) w = destSizeX_ - x;
                    if (y + h > destSizeY_) h = destSizeY_ - y;

                    return applyShaderInsecure(x, y, w, h, shader);
                }
            
            //}
            //----------------------------------------------------------------------------

        //}
        //----------------------------------------------------------------------------

    //}
    //----------------------------------------------------------------------------

//}
//----------------------------------------------------------------------------

}

#endif /*MY_SDL_RENDERER_HPP_INCLUDED*/