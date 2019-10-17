O arquivo principal é o "main_code.c".

Estamos lendo as mensagens a partir da serial, decodificando-as e enviando-as para a versão local do servidor Radarlivre. Todas as funcionalidades de rede agora estão implementadas utilizando-se a bilblioteca libcurl. Além disso, os dados enviados estão no formato JSON.

A cada 1 minuto, um hello é enviado do coletor para o servidor, através de uma requisição PUT. E, sempre que uma nova mensagem é considerada completa, ela é enviada para o servidor, através de uma requisição POST.

Um detalhe importante é que a serial parece estar sendo bloqueada durante algum tempo, depois que enviamos o Hello para o servidor. Isso precisa ser investigado.

Além disso, para rodar a aplicação, executamos o script runcolector.sh, o qual realiza as configurações necessárias para usarmos a biblioteca libcurl e dispara o executável.