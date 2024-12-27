package com.marcosoft.gamescraper.beans;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class ConfigEmu {
	
	public ConfigEmu(){
        options_before_rom = false;
        use_rom_file = false;
        use_extension = true;
        use_rom_directory = true;
    }
	
	String name;
    String description;
    String system;
    //Location of emulator, i.e. c:\mame
    String directory;
    //Name of emulator executable, i.e. mame.exe
    String executable;
    //Global options passed to emulator, i.e. -sound 1
    String global_options;
    //Options go before ROM when launching: "yes" or "no".
    // i.e. yes: "emulator.exe -option1 -option2 rom"
    //       no: "emulator.exe rom -option1 -option2"
    boolean options_before_rom;
    

    
    //Directory to ROMs
    String rom_directory;

    //List of possible ROM extensions (without the ".")
    String rom_extension;
    
    String assets;
    
    String screen_shot_directory;
    
    //# A ROM file is a list of ROMs to use.  If set to "no", ROMs are
    //# scanned for in the rom_directory.  If set to "yes" a ROM file (which
    //# is essentially just a list of ROMs) is used instead of trying scan.
    //# The default is "no".  ROM files are useful for merged ROMs with
    //# MAME, where the actual ROM names are buried within a ZIP file.
    boolean use_rom_file;
    
    //Use extension when launching game: "yes" or "no"
    // i.e. yes: "emulator.exe rom.ext"
    //       no: "emulator.exe rom"
    boolean use_extension;
    
    //Use rom_directory when launcher game: "yes" or "no"
    // i.e. yes: "emulator.exe c:\full\path\rom"
    //       no: "emulator.exe rom"
    boolean use_rom_directory;
    
    //This is the list of prescanned roms
    String map_file;
}
