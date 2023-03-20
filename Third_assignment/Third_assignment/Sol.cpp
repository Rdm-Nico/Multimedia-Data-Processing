//
//// prendo classe bitwrite del compito precedente tramite un modulo 
//// provare ad usare i moduli con file .xx --> Module Interface Unit --> inizia con export module <nome modilo>;
//// import delle librerie;
//// export classe per esportare una classe 
//
//// e poi si usa --> import <nome modluo>;
//#include<string>
//
//
//int main(int argc, char* argv[]) {
//	std::string mode = argv[1];
//
//	if (mode != "c")
//	{
//		return 1;
//	}
//
//	// apriamo i due file 
//	/*
//	creiamo un bitwriter per lo stream out
//
//	per ogni num intero dal file --> is >> val
//
//	Operazione di mapping ( che abbiamo visto )
//
//	devo contare quanti bit è composto il numero -->
//	int count = 0;
//	while(num > 0)
//	++count;
//	num  >>=1 --> scifnto di uno 
//	e poi ritorno count
//	----------
//	e poi scrivo su bitwriter con val , con (2*n -1) bit 
//
//	bw(val,2*n -1)
//
//
//}