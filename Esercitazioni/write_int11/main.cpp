#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<iterator>




int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: write_int11 <filein.txt <fileout.bin\n";
		return 1;
	}

	std::cout << argv[1];
	std::ifstream is(argv[1]);
	if (!is) {
		return 1;
	}

	std::vector<int32_t> v{
		std::istream_iterator<int32_t>(is),
		std::istream_iterator<int32_t>()
	};

	std::ofstream out(argv[2], std::ios::binary);
	if (!out)
	{
		return 1;
	}

	for (const auto& x : v) {
		std::cout << x << "\n";
	}

	return 0;
}