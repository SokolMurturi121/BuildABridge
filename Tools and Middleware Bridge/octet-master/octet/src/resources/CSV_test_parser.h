// this is a basic csv file parser. Based off of Adam Joyce's parser. It will be used to parse a simple csv file created in excel
// The parser will be used to parse a custom csv file  which will allow us to place several bridges within the scene
// additional information was gathered from http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c

// for std::ifstream
#include <fstream>
// for std::getline()
#include <string>
// for std::stringstream
#include <sstream>

namespace octet {
	class CSV_test_parser {

	public:
		CSV_test_parser() {

		}

		~CSV_test_parser() {

		}

		///this function will be used to parses a csv file. once the file is parsed we will populate a array with the data from the csv file.
		void parse_file(std::string file_path, dynarray<vec4> &arr) {
			std::ifstream file_stream(file_path);

			// ensure there are no issues opening file
			if (!file_stream) {
				std::string err = "Error unable to load the csv file";
				printf(err.c_str());
			}

			std::string line = "";

			while (file_stream.good()) {
				// load a row
				std::getline(file_stream, line, '\n');

				// this line of code dertermins the limit of each row with a comma when it is present
				line = line + ',';
				std::stringstream line_stream(line);

				vec4 data;
				std::string value = "";

				while (line_stream.good()) {
					// this line gets values from each cell in the database
					std::getline(line_stream, value, ',');

					// this checks to see if we are looking at letters or numbers if they are letters we skip the cell and look for numerical values
					if (isdigit(value[0]) || value[0] == '-') {
						const float number = (float)atof(value.c_str());
						data[0] = number;

						float num;
						for (int i = 1; i < 4; i++) {
							// during the for each loop every time we enter into a new section of the loop we get the next up to 4 in this case
							std::getline(line_stream, value, ',');

							// we must also take into account that some rows will not always fill the vector so we pad the remaining with zeros
							if (value == "") {
							
								num = NULL;
							}
							else {
								num = (float)atof(value.c_str());
							}

							const float data_num = num;
							data[i] = data_num;
						}

						arr.push_back(data);
					}
				}
			}

		}
	};
}