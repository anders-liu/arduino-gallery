using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FontGenerator
{
    class Program
    {
        const string imageFile = @"..\..\..\..\Images\Fonts2.bmp";
        const int bytesPerLine = 16;

#if NONE
        static readonly CharSettings[] charSettings =
        {
            // 24x48 font, always have 3 lines empty at top and 5 lines at bottom.
            new CharSettings { w = 24, h = 40, x = 0, y = 3, n = "0", v = "CHAR_0" },
            new CharSettings { w = 24, h = 40, x = 24, y = 3, n = "1", v = "CHAR_1" },
            new CharSettings { w = 24, h = 40, x = 48, y = 3, n = "2", v = "CHAR_2" },
            new CharSettings { w = 24, h = 40, x = 72, y = 3, n = "3", v = "CHAR_3" },
            new CharSettings { w = 24, h = 40, x = 96, y = 3, n = "4", v = "CHAR_4" },
            new CharSettings { w = 24, h = 40, x = 120, y = 3, n = "5", v = "CHAR_5" },
            new CharSettings { w = 24, h = 40, x = 144, y = 3, n = "6", v = "CHAR_6" },
            new CharSettings { w = 24, h = 40, x = 168, y = 3, n = "7", v = "CHAR_7" },
            new CharSettings { w = 24, h = 40, x = 192, y = 3, n = "8", v = "CHAR_8" },
            new CharSettings { w = 24, h = 40, x = 216, y = 3, n = "9", v = "CHAR_9" },
            new CharSettings { w = 24, h = 40, x = 240, y = 3, n = "-", v = "CHAR_NEG" },

            // 16x32 font, always have 2 lines empty at top and 3 lines at bottom.
            new CharSettings { w = 16, h = 27, x = 0, y = 50, n = "Degree", v = "CHAR_DEG" },
            new CharSettings { w = 16, h = 27, x = 16, y = 50, n = "Celsius", v = "CHAR_C" },
            new CharSettings { w = 16, h = 27, x = 32, y = 50, n = "Fahrenheit", v = "CHAR_F" },
            new CharSettings { w = 16, h = 27, x = 48, y = 50, n = "%", v = "CHAR_PERCENT" },

            // 16x16 font, always have 1 lines empty at top and 1 lines at bottom.
            new CharSettings { w = 16, h = 14, x = 64, y = 49, n = "Antenna", v = "CHAR_ANT" },
            new CharSettings { w = 16, h = 14, x = 64, y = 65, n = "Antenna Error", v = "CHAR_ANT_ERR" },
            new CharSettings { w = 16, h = 14, x = 80, y = 49, n = "Thermometer", v = "CHAR_TH" },
            new CharSettings { w = 16, h = 14, x = 80, y = 65, n = "Thermometer Error", v = "CHAR_TH_ERR" },
        };
#endif

        static readonly CharSettings[] charSettings =
        {
            // 8x16 digits.
            new CharSettings { w = 8, h = 12, x =  0, y = 18, n = "0", v = "BMP8X16_0" },
            new CharSettings { w = 8, h = 12, x =  8, y = 18, n = "1", v = "BMP8X16_1" },
            new CharSettings { w = 8, h = 12, x = 16, y = 18, n = "2", v = "BMP8X16_2" },
            new CharSettings { w = 8, h = 12, x = 24, y = 18, n = "3", v = "BMP8X16_3" },
            new CharSettings { w = 8, h = 12, x = 32, y = 18, n = "4", v = "BMP8X16_4" },
            new CharSettings { w = 8, h = 12, x = 40, y = 18, n = "5", v = "BMP8X16_5" },
            new CharSettings { w = 8, h = 12, x = 48, y = 18, n = "6", v = "BMP8X16_6" },
            new CharSettings { w = 8, h = 12, x = 56, y = 18, n = "7", v = "BMP8X16_7" },
            new CharSettings { w = 8, h = 12, x = 64, y = 18, n = "8", v = "BMP8X16_8" },
            new CharSettings { w = 8, h = 12, x = 72, y = 18, n = "9", v = "BMP8X16_9" },

            // 16x16 letters.
            new CharSettings { w = 16, h = 12, x =   0, y = 34, n = "16x16 char A", v = "BMP16X16_A" },
            new CharSettings { w = 16, h = 12, x =  16, y = 34, n = "16x16 char D", v = "BMP16X16_D" },
            new CharSettings { w = 16, h = 12, x =  32, y = 34, n = "16x16 char E", v = "BMP16X16_E" },
            new CharSettings { w = 16, h = 12, x =  48, y = 34, n = "16x16 char F", v = "BMP16X16_F" },
            new CharSettings { w = 16, h = 12, x =  64, y = 34, n = "16x16 char H", v = "BMP16X16_H" },
            // No "I", draw it as a line.
            new CharSettings { w = 16, h = 12, x =  96, y = 34, n = "16x16 char M", v = "BMP16X16_M" },
            new CharSettings { w = 16, h = 12, x = 112, y = 34, n = "16x16 char N", v = "BMP16X16_N" },
            new CharSettings { w = 16, h = 12, x = 128, y = 34, n = "16x16 char O", v = "BMP16X16_O" },
            new CharSettings { w = 16, h = 12, x = 144, y = 34, n = "16x16 char R", v = "BMP16X16_R" },
            new CharSettings { w = 16, h = 12, x = 160, y = 34, n = "16x16 char S", v = "BMP16X16_S" },
            new CharSettings { w = 16, h = 12, x = 176, y = 34, n = "16x16 char T", v = "BMP16X16_T" },
            new CharSettings { w = 16, h = 12, x = 192, y = 34, n = "16x16 char U", v = "BMP16X16_U" },
            new CharSettings { w = 16, h = 12, x = 208, y = 34, n = "16x16 char W", v = "BMP16X16_W" },

            // 24x32 digits + symbols
            new CharSettings { w = 24, h = 24, x =   0, y = 50, n = "0", v = "BMP24X32_0" },
            new CharSettings { w = 24, h = 24, x =  24, y = 50, n = "1", v = "BMP24X32_1" },
            new CharSettings { w = 24, h = 24, x =  48, y = 50, n = "2", v = "BMP24X32_2" },
            new CharSettings { w = 24, h = 24, x =  72, y = 50, n = "3", v = "BMP24X32_3" },
            new CharSettings { w = 24, h = 24, x =  96, y = 50, n = "4", v = "BMP24X32_4" },
            new CharSettings { w = 24, h = 24, x = 120, y = 50, n = "5", v = "BMP24X32_5" },
            new CharSettings { w = 24, h = 24, x = 144, y = 50, n = "6", v = "BMP24X32_6" },
            new CharSettings { w = 24, h = 24, x = 168, y = 50, n = "7", v = "BMP24X32_7" },
            new CharSettings { w = 24, h = 24, x = 192, y = 50, n = "8", v = "BMP24X32_8" },
            new CharSettings { w = 24, h = 24, x = 216, y = 50, n = "9", v = "BMP24X32_9" },

            new CharSettings { w = 24, h = 24, x = 48, y = 81, n = "C", v = "BMP24X32_C" },
            new CharSettings { w = 24, h = 24, x = 72, y = 81, n = "F", v = "BMP24X32_F" },
            new CharSettings { w = 24, h = 24, x = 96, y = 81, n = "%", v = "BMP24X32_PERCENT" },

            new CharSettings { w = 16, h = 10, x = 24, y = 81, n = "Degree", v = "BMP24X32_DEG" },
        };

        static void Main(string[] args)
        {
            using (var image = Bitmap.FromFile(imageFile) as Bitmap)
            {
                foreach (var s in charSettings)
                {
                    WriteCharVariable(image, s);
                }
            }

            Console.Write("Done. Press any key to exit...");
            Console.ReadKey(true);
        }

        private static void WriteCharVariable(Bitmap image, CharSettings s)
        {
            Console.WriteLine($"// {s.n}");
            Console.WriteLine($"const unsigned char PROGMEM {s.v}[] = {{");

            var bitc = 0;
            var bytec = 0;
            var currentByte = (byte)0;

            for (var y = 0; y < s.h; y++)
            {
                for (var x = 0; x < s.w; x++)
                {
                    var color = image.GetPixel(s.x + x, s.y + y);
                    if (color.R != 0xFF || color.G != 0xFF || color.B != 0xFF)
                    {
                        currentByte |= (byte)(0x80 >> bitc);
                    }

                    bitc++;
                    if (bitc == 8)
                    {
                        if (bytec == 0)
                        {
                            Console.Write("\t");
                        }
                        Console.Write($"0x{currentByte:X2}, ");

                        currentByte = 0;
                        bitc = 0;

                        bytec++;
                        if (bytec == bytesPerLine)
                        {
                            Console.WriteLine();
                            bytec = 0;
                        }
                    }
                }
            }

            if (bytec != 0)
            {
                Console.WriteLine();
            }

            Console.WriteLine($"}};");
            Console.WriteLine();
        }
    }

    struct CharSettings
    {
        public int w;  // Pixel Width
        public int h;  // Pixel Height
        public int x;  // X in bitmap
        public int y;  // Y in bitmap
        public string n;  // Char name in comments
        public string v;  // Variable name in generated code
    }
}
