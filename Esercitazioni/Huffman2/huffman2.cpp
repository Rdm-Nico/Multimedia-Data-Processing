#include<fstream>
#include<vector>
#include<iterator>
#include<map>
#include<iomanip>
#include<iostream>
#include<cmath>
#include<deque>
#include<algorithm>


template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast<const char*>(&val), size);
}

class bitwriter {
private:
	uint8_t buffer_;
	int n_ = 0;
	std::ostream& os_;

	std::ostream& write_bit(bool bit) {
		
		buffer_ = (buffer_ << 1) | bit;
		n_++;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0;
		}
		return os_;
	}

public:
	bitwriter(std::ostream& os): os_(os){}

	~bitwriter() {
		flush();
	}

	void flush(bool n_bit = 0) {
		while (n_ > 0) {
			write_bit(n_bit);
		}
	}

	std::ostream& write(uint32_t num, uint8_t n_bit) {
		for (uint8_t i = n_bit - 1; i >= 0; --i) {
			bool cure_bit = (num >> i) & 1;
			write_bit(cure_bit);
		}
		return os_;
	}

	std::ostream& operator()(uint32_t num, uint8_t n_bit) {
		return write(num, n_bit);
	}
};

template<typename T>

std::istream& raw_read(std::istream& is, T& val, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&val), size);
}

class bitreader {
private:
	uint8_t buffer_;
	int n_ = 0;
	std::istream& is_;

public:
	bitreader(std::istream& is): is_(is){}

	uint8_t read_bit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		n_--;
		return (buffer_ >> n_) & 1;
	}

	uint32_t read(uint8_t n_bit) {
		uint32_t u = 0;
		while(n_bit --> 0){
			u = (u << 1) | read_bit();
		}
		return u;
	}

	bool fail() const {
		return is_.fail();
	}

	explicit operator bool() const  {
		return !fail();
	}
};

// node Huffman
struct node {
	char sym_;
	float prob_;
	node* left_ = nullptr;
	node* right_ = nullptr;

	node(char sym, float prob) : sym_(sym), prob_(prob){}
	node(char sym, float prob, node* left, node* right) : sym_(sym), prob_(prob), left_(left), right_(right) {}

	bool operator<(const node* other) const  {
		return prob_ > other->prob_; // confronto con un altro nodo
	}
};


// per poter fare la sort con la lib algo dobbiamo creare la funzione

bool nodeCompare(const node* a, const node* b) {
	return a->prob_ > b->prob_;
}



// albero di huffman display
void printBT(const std::string& prefix, const node* node, bool isLeft)
{
	if (node != nullptr)
	{
		std::cout << prefix;

		std::cout << (isLeft ? "|--" : "L--");

		// print the value of the node
		std::cout << node->prob_ << std::endl;

		// enter the next tree level - left and right branch
		if (node->left_ != nullptr) {
			printBT(prefix + (isLeft ? "|   " : "    "), node->left_, true);
		}

		if (node->right_ != nullptr) {
			printBT(prefix + (isLeft ? "|   " : "    "), node->right_, false);
		}
	}
	else {
		return;
	}
}

void printBT(const node* node)
{
	printBT("", node, false);
}



void assign_code(node* node, std::map<char, std::string>& codes, std::string& seq) {

	if (node) {
		if (node->sym_ != '\0') {
			codes[node->sym_] = seq;
		}

		// chiamata ricorsiva
		seq.push_back('0');
		assign_code(node->left_, codes, seq);
		seq.pop_back();

		seq.push_back('1');
		assign_code(node->right_, codes, seq);
		seq.pop_back();
	}
}


void inc_code(std::string& code) {
	int carry = 1;
	for (int i = code.length() - 1; i >= 0 && carry; --i) {
		if (code[i] == '0'){
			code[i] = '1';
			carry = 0;
		}
		else {
			code[i] = '1';
			carry = 1;
		}
	}
	if (!carry) { // carry uguale a 0
		code = '1' + code;
	}
}

bool compression(std::ifstream& in, std::ofstream& out) {
	using namespace std;
	// creiamo il bitwriter e bitreader 
	bitwriter bw(out);
	bitreader br(in);
	if (!in) {
		return false;
	}

	string Magic_Number = "HUFFMAN2";
	
	out << Magic_Number;

	

	// calcoliamo la freq dei simboli
	map<char, float> m;

	char carattere;
	while (in >> carattere)
	{
		m[carattere]++;
	}
	
	uint32_t sum = 0;

	for (auto it = m.begin(); it != m.end(); it++) {
		cout << it->first << "\t" << it->second << endl;
		sum += it->second;
	}

	for (auto it = m.begin(); it != m.end(); it++) {
		it->second /= sum;
	}

	cout << "Normalizzati: " << endl;

	for (auto it = m.begin(); it != m.end(); it++) {
		cout << it->first << "\t" << it->second << endl;
		sum += it->second;
	}

	// inseriamo il numero di elmenti
	if (m.size() == 256)
		out << 0;
	
	out << (uint8_t)m.size();

	vector<node*> nodeptr_r;

	
	for (auto it = m.begin(); it != m.end(); it++) {
		node* n = new node(it->first, it->second);
		nodeptr_r.push_back(n);
	}


	cout << "start encoding:" << endl;

	node* root;

	while (nodeptr_r.size() > 1) {
		sort(nodeptr_r.begin(), nodeptr_r.end(), nodeCompare);
		/* si poteva anche utilizzare una versione con una funzione gamma:
		* sort(nodeptr_r.begin(), nodeptr_r.end(),[](node* a, node*b
		{	 return a-->prob_ > b-->prob_;});	*/



		node* l = nodeptr_r.back();
		nodeptr_r.pop_back();
		node* r = nodeptr_r.back();
		nodeptr_r.pop_back();

		// creiamo il nuovo nodo come la  somma delle prob dei due e un simbolo sconosciutto 
		node* parent = new node('\0', l->prob_ + r->prob_, l, r);

		nodeptr_r.push_back(parent);
	}

	root = nodeptr_r.front();

	cout << "end encdoing" << endl;

	cout << "Huffman tree visual" << endl;

	
	printBT(root);


	map<char, string > huffman_codes;

	string seq = "";

	assign_code(root, huffman_codes, seq);


	vector<pair<char, string>> sorted_huff_codes; 
	vector<pair<char, string>> canonical_huff_codes;


	for (auto& it : huffman_codes) {
		sorted_huff_codes.push_back(make_pair(it.first,it.second));
	}

	// questa sort permtte di fare il confronto tra due stringhe tramite la loro lunghezza, con una lambda function 
	sort(sorted_huff_codes.begin(), sorted_huff_codes.end(), [](const pair<char, string>& a, const pair<char, string>& b) {
		return a.second.length() > b.second.length();
		});

	cout << "vettore di Huffman ordinato: " << endl;

	for (auto& it : sorted_huff_codes) {
		cout << it.first << "\t" << it.second << endl;
	}

	int code_length = 0;

	string code = ""; // nuovo codice  per huffman canonico

	for (vector<pair<char, string>>::iterator it = sorted_huff_codes.begin(); it != sorted_huff_codes.end(); it++) {

		// assegna i codici di huffman canonici ai simboli ordinati
		while (code.length() < it->second.length()) {
			code += '0'; // inseriamo uno 0
		}
		// inseriamo dentro al vettore canonico 
		canonical_huff_codes.push_back(make_pair(it->first, code));

		inc_code(code); // incrementiamo il codice
	}


	for (auto& it : canonical_huff_codes) {
		cout << it.first << "\t" << it.second << endl;
		// scriviamo sul file il simbolo 
		bw(static_cast<uint8_t>(it.first), 8);
		uint8_t len = it.second.length();

		bw(len, 5);

	}

	// numero di simboli codificati
	bw(sum,32);

	// inseriamo i caratteri per l'huffman
	in.clear();
	in.seekg(0,in.beg); // beg == inizio di uno stream

	map<char, string> can_map;

	// facciamo la copia della mappa
	copy(canonical_huff_codes.begin(), canonical_huff_codes.end(), inserter(can_map, can_map.begin()));

	char c;
	while (in >> c) {
		for (size_t i = 0; i < can_map[c].length(); i++) {
			uint8_t bit = can_map[c].c_str()[i] - '0';
			cout << c << "\t" << can_map[c] << endl;
			bw.write(bit, 1);
		}
	}


	return true;
}

bool decompression(std::ifstream& is, std::ofstream& out) {
	
	return true;
}

int main(int argc, char* argv[]) {
	using namespace std;
	if (argc != 4) {
		cout << "Usage: huffman2 [c|d] <input file> <output file>" << endl;
		return EXIT_FAILURE;
	}
	
	ifstream in(argv[2], ios::binary);
	ofstream out(argv[3], ios::binary);
	if (!in || !out) {
		cout << "Error opening the file" << endl;
		return	EXIT_FAILURE;
	}

	char mod = *argv[1];

	if (mod != 'c' && mod != 'd') {
		cout << "Only [c|d} accpeted";
		return EXIT_FAILURE;
	}

	mod == 'c' ? compression(in, out) : decompression(in, out);

	return EXIT_SUCCESS;
}