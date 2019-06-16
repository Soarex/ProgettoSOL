#include "../../Client/src/client.h"

int main(int argc, char** argv) {
	if (argc != 3) {
		LOG("Usage: nome #test", ERROR);
		exit(EXIT_FAILURE);
	}

	char* nome = argv[1];
	int numeroTest = atoi(argv[2]);
	int numeroOperazioni = 0;
	int operazioniSuccesso = 0;
	int operazioniErrore = 0;
	char nomeBlocco[20]; char* data;

	if (!os_connect(nome)) exit(EXIT_FAILURE);
	switch (numeroTest) {
	case 1:
		for (int i = 0; i < 20; i++) {
			sprintf(nomeBlocco, "Blocco_%d", i);
			data = malloc(100 + 5000 * i);

			for (int j = 0; j < 100 + 5000 * i; j++)
				data[j] = j % 256;
			
			if (os_store(nomeBlocco, (void*)data, 100 + 5000 * i))
				operazioniSuccesso++;
			else
				operazioniErrore++;

			numeroOperazioni++;
			free(data);
		}
		break;

	case 2:
		for (int i = 0; i < 20; i++) {
			sprintf(nomeBlocco, "Blocco_%d", i);
			data = malloc(100 + 5000 * i);

			LOG("Prima", INFO);
			if (!(data = (char*)os_retrieve(nomeBlocco))) {
				operazioniErrore++;
				numeroOperazioni++;
				break;
			}
			LOG("dopo", INFO);
	
			int test;
			for (int j = 0; j < 100 + 5000 * i; j++)
				test += data[j] != j % 256;
			if(test == 0)
				operazioniSuccesso++;
			else
				operazioniErrore++;
			numeroOperazioni++;
			free(data);
		}
		break;

	case 3:
		for (int i = 0; i < 20; i++) {
			sprintf(nomeBlocco, "Blocco_%d", i);
			
			if (os_delete(nomeBlocco))
				operazioniSuccesso++;
			else
				operazioniErrore++;

			numeroOperazioni++;
		}
		break;

	default:
		LOG("Test non esistente", ERROR);
		exit(EXIT_FAILURE);
		break;
	}

	char buffer[100];
	sprintf(buffer, "Test effettuati: %d, Successo: %d, Falliti: %d", numeroOperazioni, operazioniSuccesso, operazioniErrore);
	LOG(buffer, INFO);
	os_disconnect();
	return 0;
}