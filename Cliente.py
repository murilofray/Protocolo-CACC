import socket

HOST = "127.0.0.1"
PORT = 80

def conectarServidor(HOST, PORT):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    return s

def enviarMensagem(s, mensagem):
    s.sendall(bytes(mensagem, 'ascii'))

def receberResposta(s):
    resposta = s.recv(1024)
    return resposta.decode('ascii')

def fecharConexao(s):
    s.close()

def enviarRequisicao(HOST, PORT, mensagem):
    s = conectarServidor(HOST, PORT)
    enviarMensagem(s, mensagem)
    resposta = receberResposta(s)
    fecharConexao(s)
    return resposta

def criarRequisicao(tipo, documento):
    return f"{tipo};{len(documento)};{documento}"

def interpretarResposta(resposta):
    partes = resposta.split(';')
    tipo = int(partes[0])
    tamanho = int(partes[1])
    documento = partes[2]
    status = int(partes[3])

    if status == 1 or status == 2:
        if tipo == 1:
            tipo_documento = "CPF"
        elif tipo == 2:
            tipo_documento = "CNPJ"
        if status == 1:
            mensagem = f"{tipo_documento} válido."
        elif status == 2:
            mensagem = f"{tipo_documento} inválido."
    elif status == 3:
        mensagem = "Tamanho de documento inválido."
    elif status == 4:
        mensagem = "Tipo não suportado."
    elif status == 5:
        mensagem = "Erro na requisição."
    elif status == 6:
        mensagem = "Valores não permitidos nos campos da requisição."
    else:
        mensagem = "Status desconhecido."

    return mensagem

print("Digte o tipo do documento:\n 1-CPF 2-CNPJ:")
tipo = input()
print("Digte o numero do documento:")
documento = input()
resposta = interpretarResposta(enviarRequisicao(HOST, PORT, criarRequisicao(tipo,documento)))
print(resposta)