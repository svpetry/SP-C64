using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RomBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            var rom = new byte[0xFFFF];

            AddRom(rom, "kernal.901227-03.bin", 0xA000); // kernal 1
            AddRom(rom, "JiffyDOS_C64.bin", 0x2000); // kernal 2
            AddRom(rom, "EXOS.ROM", 0x8000); // kernal 3
            AddRom(rom, "CockroachROM.bin", 0x0000); // kernal 4

            AddRom(rom, "basic.901226-01.bin", 0xC000);
            AddRom(rom, "characters.901225-01.bin", 0x6000);

            File.WriteAllBytes("ROM.bin", rom);
        }

        private static void AddRom(byte[] rom, string fileName, int address)
        {
            var fileBytes = File.ReadAllBytes(fileName);
            for (var i = 0; i < fileBytes.Length; i++)
                rom[address + i] = fileBytes[i];
        }
    }
}
