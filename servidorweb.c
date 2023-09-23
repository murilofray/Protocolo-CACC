#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 80
#define MESSAGESIZE 1024
#define MAXSOCKETS 100

typedef struct sLista
{
    SOCKET info;
    struct sLista* next;
} LISTA;

LISTA *l = NULL;

int vazia()
{
    if(l == NULL)
        return 1;
    return 0;
}

LISTA* getNode()
{
    return (LISTA*)malloc(sizeof(LISTA));
}

void freeNode()
{
    free(l);
}

void inserirInicio(SOCKET newSocket)
{
    LISTA *aux;
    aux = getNode();
    if(aux == NULL)
    {
        printf("ERRO AO ALOCAR MEMORIA\n");
        return;
    }
    aux->info = newSocket;
    aux->next = l;
    l = aux;
}

void removerSocket(SOCKET socketRemover)
{
    LISTA *atual = l;
    LISTA *anterior;
    while (atual != NULL)
    {
        if (atual->info == socketRemover)
        {
            if (anterior != NULL)
                anterior->next = atual->next;
            else
                l = atual->next;
            free(atual);
            return;
        }
        anterior = atual;
        atual = atual->next;
    }
}


int validarCPF(char cpf[])
{
    int i, j, digito1 = 0, digito2 = 0;
    if(strlen(cpf) != 11)
        return 2;
    else if((strcmp(cpf,"00000000000") == 0) || (strcmp(cpf,"11111111111") == 0) || (strcmp(cpf,"22222222222") == 0) ||
            (strcmp(cpf,"33333333333") == 0) || (strcmp(cpf,"44444444444") == 0) || (strcmp(cpf,"55555555555") == 0) ||
            (strcmp(cpf,"66666666666") == 0) || (strcmp(cpf,"77777777777") == 0) || (strcmp(cpf,"88888888888") == 0) ||
            (strcmp(cpf,"99999999999") == 0))
        return 2; ///se o CPF tiver todos os números iguais ele é inválido.
    else
    {
        ///digito 1---------------------------------------------------
        for(i = 0, j = 10; i < strlen(cpf)-2; i++, j--) ///multiplica os números de 10 a 2 e soma os resultados dentro de digito1
            digito1 += (cpf[i]-48) * j;
        digito1 %= 11;
        if(digito1 < 2)
            digito1 = 0;
        else
            digito1 = 11 - digito1;
        if((cpf[9]-48) != digito1)
            return 2; ///se o digito 1 não for o mesmo que o da validação CPF é inválido
        else
            ///digito 2--------------------------------------------------
        {
            for(i = 0, j = 11; i < strlen(cpf)-1; i++, j--) ///multiplica os números de 11 a 2 e soma os resultados dentro de digito2
                digito2 += (cpf[i]-48) * j;
            digito2 %= 11;
            if(digito2 < 2)
                digito2 = 0;
            else
                digito2 = 11 - digito2;
            if((cpf[10]-48) != digito2)
                return 2; ///se o digito 2 não for o mesmo que o da validação CPF é inválido
        }
    }
    return 1;
}

int validarCNPJ(char cnpj[])
{
    int sm, i, r, num, mult;
    char dig13, dig14;

// calcula o 1o. digito verificador do CNPJ
    sm = 0;
    mult = 2;
    for (i=11; i>=0; i--)
    {
        num = cnpj[i] - 48;	// transforma o caracter '0' no inteiro 0
        // (48 eh a posição de '0' na tabela ASCII)
        sm = sm + (num * mult);
        mult = mult + 1;
        if (mult == 10)
            mult = 2;
    }
    r = sm % 11;
    if ((r == 0) || (r == 1))
        dig13 = '0';
    else
        dig13 = (11 - r) + 48;

// calcula o 2o. digito verificador do CNPJ
    sm = 0;
    mult = 2;
    for (i=12; i>=0; i--)
    {
        num = cnpj[i] - 48;
        sm = sm + (num * mult);
        mult = mult + 1;
        if (mult == 10)
            mult = 2;
    }
    r = sm % 11;
    if ((r == 0) || (r == 1))
        dig14 = '0';
    else
        dig14 = (11 - r) + 48;

// compara os dígitos calculados com os dígitos informados
    if ((dig13 == cnpj[12]) && (dig14 == cnpj[13]))
        return 1;
    else
        return 2;
}

int validarEntradaNumerica(const char* entrada)
{
    int i;
    for (i = 0; i < strlen(entrada); i++)
    {
        if (!isdigit(entrada[i]))
        {
            return 0;
        }
    }
    return 1;
}

void getdata(SOCKET s)
{
    int len;
    char requisicao[MESSAGESIZE];
    char resposta[MESSAGESIZE];
    while (1)
    {
        requisicao[0] = '\0';
        len = recv(s, requisicao, MESSAGESIZE, 0);
        if (len > 0)
        {
            requisicao[len] = '\0';
            char* tipo = strtok(requisicao, ";");
            char* tamanho = strtok(NULL, ";");
            char* ndoc = strtok(NULL, ";");
            char* campoExtra = strtok(NULL, ";");

            if (tipo != NULL && tamanho != NULL && ndoc != NULL && campoExtra == NULL)
            {
                if (validarEntradaNumerica(tipo) && validarEntradaNumerica(tamanho) && validarEntradaNumerica(ndoc))
                {
                    int tipoDocumento = atoi(tipo);
                    if(tipoDocumento != 1 && tipoDocumento != 2)
                    {
                        sprintf(resposta, "0;0;0;4");
                    }
                    else if (tipoDocumento == 1 && strlen(ndoc) == 11)
                    {
                        int resultado = validarCPF(ndoc);
                        sprintf(resposta, "%d;%d;%s;%d", tipoDocumento, strlen(ndoc), ndoc, resultado);
                    }
                    else if (tipoDocumento == 2 && strlen(ndoc) == 14)
                    {
                        int resultado = validarCNPJ(ndoc);
                        sprintf(resposta, "%d;%d;%s;%d", tipoDocumento, strlen(ndoc), ndoc, resultado);
                    }
                    else
                    {
                        sprintf(resposta, "%d;%d;%s;3", tipoDocumento, strlen(ndoc), ndoc);
                    }
                }
                else
                {
                    sprintf(resposta, "0;0;0;6");
                }
            }
            else
            {
                sprintf(resposta, "0;0;0;5");
            }

            send(s, resposta, strlen(resposta), 0);
            removerSocket(s);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server, client;
    int c, pos;
    char errormessage[MESSAGESIZE];

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    if((s = socket(AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET)
    {
        printf("\nNao e possivel inicializar o socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
    if( bind(s,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("\nNao e possivel construir o socket: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    listen(s,3);
    c = sizeof(struct sockaddr_in);
    printf("*** SERVER ***\n\nAguardando conexoes...\n\n");

    while(1)
    {
        inserirInicio(accept(s, (struct sockaddr *)&client, &c));
        if (l->info == INVALID_SOCKET)
        {
            printf("\nConexao não aceita. Codigo de erro: %d", WSAGetLastError());
        }
        else
        {
            puts("\nConexao aceita.");
            printf("\nDados do cliente - IP: %s -  Porta: %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
            _beginthread(getdata,NULL,l->info);
        }
    }
    closesocket(s);
    WSACleanup();
}
