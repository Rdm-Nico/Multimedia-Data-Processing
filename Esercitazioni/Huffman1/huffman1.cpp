#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
#include<iterator>
#include<map>
#include<algorithm>

using namespace std;
template<typename T>
istream& raw_read(istream& is, T& value, size_t size = sizeof(T)) {
	return is.read(reinterpret_cast<char*>(&value), size);
}
class bitreader {
private:
	uint8_t buffer_;
	uint8_t n_ = 0;
	istream& is_;

public:
	bitreader(istream& is): is_(is){}

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
		while (n_bit-- > 0) {
			u = (u << 1) | read_bit();
		}
		return u;
	}

	bool fail() const {
		return is_.fail();
	}
	explicit operator bool() const {
		return !fail();
	}
};

template<typename T>
ostream& raw_write(ostream& os,const T& value, size_t size = sizeof(T)) {
	return os.write(reinterpret_cast< const char*>(&value), size);
}

class bitwriter {
private:
	uint8_t buffer_;
	uint8_t n_ = 0;
	ostream& os_;

	ostream& write_bit(uint8_t bit) {
		buffer_ = (buffer_ << 1) | bit;
		n_++;
		if (n_ == 8)
		{
			raw_write(os_, buffer_);
			n_ = 0;
		}
		return os_;
	}
public:
	bitwriter(ostream& os) : os_(os) {}
	~bitwriter() {
		flush();
	}
	ostream& flush(bool bit = 0) {
		while (n_ > 0) {
			write_bit(bit);
		}
		return os_;
	}

	ostream& write(uint32_t num, uint8_t n_bit) {
		for (int i = n_bit - 1; i >= 0; --i) {
			bool cure_bit = (num >> i) & 1;
			write_bit(cure_bit);
		}
		return os_;
	}
	ostream& operator()(uint32_t num, uint8_t n_bit){ 
		return write(num, n_bit);
	}
};

// nodo di Huffman
struct node {
	char sym_; // simbolo 
	float prob_; // freq
	node* left_;
	node* right_;

	node(char sym, float prob, node* left, node* right): sym_(sym), prob_(prob), left_(left), right_(right){}

	bool operator<(const node& other) const { // fa il confronto tra un nodo ed un altro
		return prob_ > other.prob_;
	}
};



// per poter fare la sort e' necessaria questa funzione
bool nodePtrCompare(const node* a, const node* b) {
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

void assign_code(map<char, string>& codes, node* node, string& seq) {
	if (node) {

		if (node->sym_ != '\0') {
			codes[node->sym_] = seq;
		}

		seq.push_back('0');
		assign_code(codes, node->left_, seq);
		seq.pop_back();

		seq.push_back('1');
		assign_code(codes, node->right_, seq);
		seq.pop_back();
	}
}


int main(int argc, char* argv[]) {
	if (argc != 4) {
		cout << "Usage: huffman1 [c|d] <input file> <output file>";
		return 1;
	}

	auto mod = *argv[1];

	switch (mod)
	{
	case 'c': {
		// codifica
		ifstream in(argv[2], ios::binary);
		ofstream out(argv[3], ios::binary);
		if (!in || !out) {
			return -1;
		}
		bitreader br(in);
		bitwriter bw(out);

		if (!br) {
			cout << "Error bitreader" << endl;
			return -1;
		}
		
		out << "HUFFMAN1";

		map<char, float> m;
		char carattere;

		// calcoliamo le freq:
		while (in >> carattere)
		{
			m[carattere]++;
		}

		uint32_t sum = 0; // sommatoria di tutte le freq
		
		for (auto it = m.begin(); it != m.end(); it++) {
			cout << it->first << " " << it->second << "; ";
			sum += it->second;
		}
		cout << endl;

		for (auto it = m.begin(); it != m.end(); it++) {
			it->second/=sum;
		}

		cout << "Normalizzati:" << endl;

		for (auto it = m.begin(); it != m.end(); it++) {
			cout << it->first << " " << it->second << "; ";
		}

		if (m.size() == 256) {
			out << 0;
		}
		out << (uint8_t)m.size(); // passo il numero di elmenti 

		vector<node*> nodeptr_v; // vettore di nodi di Huffman

		int i = 0;

		for(auto it= m.begin(); it != m.end(); it++){
			node* t = new node(it->first, it->second, nullptr, nullptr);
			nodeptr_v.push_back(t);
		}

		cout << "inizio encoding:" << endl;

		node* root;


		while (nodeptr_v.size() > 1) { // finche' non  ottengo un unico nodo dentro al vettore
			sort(nodeptr_v.begin(), nodeptr_v.end(), nodePtrCompare); // ordiniamo il vettore

			cout << endl << "v_size: " << nodeptr_v.size() << endl;

			for (auto& t : nodeptr_v) {
				cout << t->prob_ << "\t" << t->sym_ << endl;
			}


			node* l = nodeptr_v.back();
			nodeptr_v.pop_back();
			node* r = nodeptr_v.back();
			nodeptr_v.pop_back();

			node* parent = new node('\0', l->prob_ + r->prob_, l, r); // creaimo il nodo padre che avra come left e right i due nodi piu bassi di freq nel vettore

			nodeptr_v.push_back(parent);
		}

		cout << "fine encoding" << endl;

		root = nodeptr_v.front();

		cout << endl << "Huffman visual tree:" << "\n\n";

		printBT(root);

		map <char, string> huff_codes; // mappa per tenere i codici di huffman creati

		string seq = "";
		assign_code(huff_codes, root, seq);

		cout << "codici di huffman:" << endl;
		for (auto& it : huff_codes) {
			cout << it.first << "\t" << it.second << endl;


			bw(it.first, 8); // scriviamo il simbolo
			uint32_t code_len = it.second.length();
			bw(code_len, 5);

			for (size_t i = 0; i < it.second.length(); i++) {
				uint8_t bit = it.second.c_str()[i] - '0'; // viene selezionato il bit e viene convertito in un intero a 8 bit
				cout << it.second.c_str()[i];
				bw(bit, 1);
			}
			cout << endl;
		}

		bw(sum, 32); // numero di simboli 

		// in questo modo evito di caricare un file su un vettore interamente 
		in.clear();
		in.seekg(0, in.beg); // beg :l'inizio di uno stream

		while (in >> carattere) { // preleva il carattere dal file iniziale 
			for (size_t i = 0; i < huff_codes[carattere].length(); i++)
			{
				uint8_t bit = huff_codes[carattere].c_str()[i] - '0';
				cout << huff_codes[carattere].c_str()[i] << endl;
				bw(bit, 1);
			}
			cout << endl;
		}

		break;
	}
	case 'd': {
		// decompressione
		ifstream in(argv[2], ios::binary);
		ofstream out(argv[3], ios::binary);
		if (!in || !out) {
			return -1;
		}
		bitwriter bw(out);
		bitreader br(in);

		string magic_number;

		while (magic_number.length() != 8) {
			char c = br.read(8);
			magic_number.push_back(c);
		}
		if (magic_number != "HUFFMAN1"){
			cout << "errore nel file passato" << endl;
		}

		uint8_t table_entries = 0;

		table_entries = br.read(8);

		table_entries - table_entries == 0 ? 256 : table_entries;

		cout << "entries: " << (uint32_t)table_entries << endl;

		map<string, uint8_t> huffman_codes;

		for (uint8_t  i = 0; i < table_entries; i++)
		{
			char sym = br.read(8);
			uint8_t len = br.read(5);
			string code = "";
			for (uint8_t j = 0; j < len; j++) {
				char bit = br.read(1);
				code.push_back(bit + '0'); // prendere il numero intero 
			}

			huffman_codes.insert({ code, sym });
		}

		uint32_t numSyml = br.read(32);
		vector<char> v;
		string r_code = "";

		for (uint32_t i = 0; i < numSyml;) {

			uint8_t bit = br.read(1);
			r_code.push_back(bit + '0');

			if (huffman_codes.count(r_code) == 1) { // conferma la condizione quando la chiave (r_code) e' stata trovata
				cout << (char)huffman_codes[r_code] << endl;
				out << huffman_codes[r_code];
				r_code = ""; // resettiamo per il prossimo 
				i++;
			}
		}


		break;
	}
	default:
		cout << "Usage [c,d]" << endl;
		return -1;
		break;
	}
	return 0;
}