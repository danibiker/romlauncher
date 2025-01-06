#pragma once

#include <map>
#include <vector>
#include <string>

#include "utils/beans/structures.h"
#include "utils/const/Constant.h"
#include "utils/io/dirutil.h"

using namespace std;

class DosNames{
    public:
        DosNames(){
        }
        ~DosNames(){
        }

        // Create a map with string keys and integer values
        void convertirNombresCortos(vector<unique_ptr<GameFile>> &listGames) {
            std::map<std::string, int> nombresCortos;
            for (size_t i=0; i < listGames.size(); i++){
                auto game = listGames.at(i).get();
                string nombreCorto = convertirAFormato83(game->longFileName, nombresCortos);
                //cout << "largo: " << game->longFileName << " corto: " << nombreCorto << endl;
                game->shortFileName = nombreCorto;
            }
        }    

        string convertirAFormato83(string nombre, std::map<std::string, int> &nombresCortos) {
            string nombreFinal = "";
            int counter = 1;
            size_t nDigits = 1;
            dirutil dir;

            string extension = dir.getExtension(nombre);
            string fileNoExt = dir.getFileNameNoExt(nombre);
            string nonascci = stripNonAsciiCharacters(fileNoExt);
            string strippedFilename = stripDots(nonascci);

            if (fileNoExt.length() <= 8 && !hasSpecialAllowedChars(fileNoExt, allowedChars)) {
                string ret = nonascci + extension;
                Constant::upperCase(&ret);
                return ret;
            }

            string shortfn = first6Characters(strippedFilename, stripNonAsciiCharacters(nombre));

            auto it = nombresCortos.find(shortfn + "~" + Constant::TipoToStr(counter));

            while (it != nombresCortos.end()) {
                counter++;
                // if more digits are added to counter, shorten shortfn by 1
                // e.g. if counter comes to 9 and shortf~9.txt is taken. try short~10.txt next
                if (nDigits != Constant::TipoToStr(counter).length() && shortfn.length() > 1) {
                    nDigits = Constant::TipoToStr(counter).length();
                    shortfn = shortfn.substr(0, shortfn.length() - 1);
                }
                it = nombresCortos.find(shortfn + "~" + Constant::TipoToStr(counter));
            }
            Constant::upperCase(&extension);
            nombreFinal = (shortfn + "~" + Constant::TipoToStr(counter) + extension);
            nombresCortos.emplace((shortfn + "~" + Constant::TipoToStr(counter)), counter);
            return nombreFinal;
        }

    private:
        const string allowedChars = "$%'`-@{}~!#()&_^";
        const std::map<std::string, std::string> hashToReplace = {{",", "_"}};

        bool hasSpecialAllowedChars(string target, string specialChars) {
            for (char ch : specialChars) {
                if (target.find(ch) != string::npos) {
                    return true; // Found a special character in the target string
                }
            }
            return false; // No special character found
        }

        string stripNonAsciiCharacters(string input) {
            string output = "";
            for (char c : input) {
                //if ((c <= 127 || allowedChars.find(c) != string::npos || (c >= 0x00A1)) && c != ' ') {
                if ((allowedChars.find(c) != string::npos) && c != ' ') {
                    string tmpC = Constant::toString(c);
                    //if (c < 0x00A1){
                    //    Constant::upperCase(&tmpC);
                    //}
                    
                    auto it = hashToReplace.find(tmpC);

                    if (it != hashToReplace.end()) {
                        output.append(it->second);
                    } else {
                        output.append(tmpC);
                    }
                }
            }
            return output;
        }


        string stripDots(string input) {
            return Constant::replaceAll(input, ".", "");
        }


        string first6Characters(string input, string nonascii) {
            if (nonascii.find_last_of(".") != nonascii.find_first_of(".") && nonascii.find_first_of(".") > 0
                    && nonascii.find_first_of(".") < 6) {
                return stripDots(nonascii.substr(0, nonascii.find_first_of(".")));
            }
            
            if (input.length() > 6)
                return input.substr(0, 6);
            else 
                return input;
        }
};