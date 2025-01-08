# romlauncher
 A multisystem rom launcher primarily made to work in MSDOS
 
Instructions
------------

Problems in linux
-----------------
I had a lot of problems with ALSA freezing the retroarch app and not playing after launching a rom. So, I found that pipewire resolved all my issues. To install it, just type the following and reboot your system afterwards:

```
sudo pacman -S pipewire pipewire-alsa pipewire-pulse
systemctl --user enable pipewire pipewire-pulse
systemctl --user start pipewire pipewire-pulse    
```

If you can't install pipewire and want to use Alsa, I provide the file .asoundrc in the directory romlauncher/gamemenu/gmenu. It must be placed on your home directory ~/.asoundrc

It's content is the following (update "card 1" with the corresponding number for your system):
```
pcm.!default {
        type hw
        card 1
}

ctl.!default {
        type hw
        card 1
}
```

Be aware that the sound stops playing after a rom is launched in retroarch. 
Also it's mandatory to edit the file "gmenu.cfg" and add the following line:

```
alsa_reset = yes
```

Compiling
---------------
See [COMPILING.md](COMPILING.md)

