/***************************************************************************
*
*  M�dulo de implementa��o: CED  Controlador de espa�os de dados alocados dinamicamente
*
*  Arquivo gerado:          CESPDIN.c
*  Letras identificadoras:  CED
*  N�mero identificador:    
*
*  Nome l�gico da base:  Arcabou�o para a automa��o de testes de programas redigidos em C
*  Nome do arquivo base: D:\AUTOTEST\PROJETOS\ARCABOUC.BSW
*
*     INF 1301 / 1628 Automatiza��o dos testes de m�dulos C
*  Direitos autorais da base
*     LES/DI/PUC-Rio
*
*  Estado corrente de distribui��o da base
*     Vers�o: 2    Modifica��o:      Altera��o:      Data libera��o: 
*
*     Autorizou libera��o     
*
*  Hist�rico do projeto
*     Vers Modf Altr  Autor    Data     Observa��es
*     4       avs   01/fev/2006 criar linguagem script simb�lica
*     3       avs   08/dez/2004 uniformiza��o dos exemplos
*     2       avs   07/jul/2003 unifica��o de todos os m�dulos em um s� projeto
*     1       avs   16/abr/2003 in�cio desenvolvimento
*  Altera��es de emerg�ncia realizadas e ainda n�o passadas a limpo
*  Altera��es e corre��es realizadas
*
***************************************************************************/

/******************* Declara��es encapsuladas do m�dulo *******************/

   /****** Inclus�es utilizadas pelo m�dulo ******/

   #include    <malloc.h>
   #include    <stdio.h>
   #include    <memory.h>
   #include    <string.h>

   /****** Inclus�o do m�dulo de defini��o pr�prio ******/

   #define CESPDIN_PROPRIO
   #include "cespdin.h"
   #undef CESPDIN_PROPRIO

   /****** Declara��es internas ao m�dulo ******/

   #include    "generico.h"
   #include    "geraalt.h"

   #include    "..\tabelas\IdTiposEspaco.def"

       /* O pragma � necess�rio para assegurar alinhamento a byte,
          uma vez que toda a aritm�tica com ponteiros assume isso */

   #pragma pack (1)

   #define     DIM_NOME_ARQ_FONTE  32
   #define     DIM_PONTEIRO         4

   #define     TIPO_MSG_INFO     "!!!"
   #define     TIPO_MSG_ERRO     "ee>"
   #define     TIPO_MSG_VAZIA    "   "

   #define     CHAR_ALOC      '|'
   #define     CHAR_DESALOC   '?'

   #define     LIMITE_FREQUENCIA      1000
   #define     LIMITE_INF_FREQUENCIA    10
   #define     LIMITE_SUP_FREQUENCIA  1000

   /* Controle do conte�do do espa�o alocado
   *
   * Ao serem alocados, a parte Valor dos espa�os � inicializados para
   * CHAR_ALOC ("`"). Ao serem desalocados, os espa�o todo � preenchido
   * com CHAR_DESALOC ("!"). Isto permite verificar o estado do espa�o
   * ao acess�-lo. O objetivo � provocar erros sistem�ticos caso um
   * programa tente utilizar espa�os desalocados, ou se esque�a de
   * inicializar campos que contenha ponteiros.
   */

   #define     DIM_VALOR      2
   #define     DIM_CONTROLE   2
   #define     CONTROLE       "\x96\xC3"
   #define     CONTROLE_ERRO  "\x01\0"

   /* Todos os espa�os s�o antecedidos e sucedidos por strings constantes
   * de controle (CONTROLE). Isto permite verificar se houve extravas�o
   * ao transferir dados para o espa�o.
   */

   #define     IMPRESSAO_DIGITAL  3141593526

   /* Todos os espacos de dados teem esta marca digital.
   * Isto permite assegurar que se trata de um espa�o controlado.
   * O que, por sua vez permite misturar m�dulos instrumentados
   * com m�dulos otimizados (n�o instrumentados) sem que ocorram erros
   * de processamento que acabam levando ao cancelamento do programa
   */

   /***** Tipos de dados internos ao m�dulo *****/

/***********************************************************************
*
*  Tipo de dados: CED Elemento da lista de controle de espa�os de dados
*
*  Descri��o do tipo
*
*     Esta estrutura define a organiza��o de um espa�o de dados
*     alocado pelo controlador de vazamento.
*     O elemento prev� campos para a identifica��o do tipo do espa�o de
*     dados e para o controle de extravas�o dos limites estabelecidos
*     para o espa�o de dados �til.
*
***********************************************************************/

      typedef struct tgElemListaControle {

      /* Estrutura de dados : Raiz de CED Elemento da lista de controle de espa�os de dados */


         /* Estrutura de dados : Marcador de espa�o controlado */

            unsigned long ImpressaoDigital ;

         /* Estrutura de dados : Tamanho total do espa�o */

            int tamTotal ;

         /* Estrutura de dados : Ponteiro para o elemento anterior na lista de espa�os alocados */

            struct tgElemListaControle * pAnt ;

         /* Estrutura de dados : Ponteiro para o elemento a seguir na lista de espa�os alocados */

            struct tgElemListaControle * pProx ;

         /* Estrutura de dados : Identificador do espa�o */

            int idEspaco ;

         /* Estrutura de dados : Marca de espa�o ativo */

            int ehAtivo ;

         /* Estrutura de dados : Identificador do tipo do valor contido no espa�o alocado */

            int idTipoValor ;

         /* Estrutura de dados : Dimens�o, em bytes, do valor contido no espa�o alocado */

            int tamValor ;

         /* Estrutura de dados : N�mero da linha do c�digo fonte */

            int numLinhaFonte ;

         /* Estrutura de dados : Nome arquivo fonte */

            char NomeArquivoFonte[ DIM_NOME_ARQ_FONTE ] ;

         /* Estrutura de dados : Marcador de controle de extravas�o antes do espa�o */

            char ControleAntes[ DIM_CONTROLE ] ;

         /* Estrutura de dados : Dummy a ser substitu�do pelo espa�o a ser ocupado pelo valor */

            char Valor[ DIM_VALOR ] ;

         /* Estrutura de dados : Marcador de controle de extravas�o ap�s */

            char ControleApos[  DIM_CONTROLE ] ;

      } tpElemListaControle ;

   /***** Estruturas de dados internas ao m�dulo *****/

   /* Estrutura de dados : *CED  Dados globais encapsulados */


      /* Estrutura de dados : Constantes l�gicas */

         #define  TRUE     1
         #define  FALSE    0

      /* Estrutura de dados : Controle de inicializa��o */

         static int estaInicializado = FALSE ;

      /* Estrutura de dados : Origem da lista de espa�os alocados */

         static tpElemListaControle * pOrgLista = NULL;

      /* Estrutura de dados : Iterador: espa�o corrente */

         static tpElemListaControle * pEspacoCorr = NULL;

      /* Estrutura de dados : Contador de espa�os alocados */

         static long numEspacosAlocados = 0 ;

      /* Estrutura de dados : Contador do total de chamadas de aloca��o */

         static long TotalAlocacoes = 0 ;

      /* Estrutura de dados : Espa�o total alocado */

         static long EspacoTotalAlocado = 0 ;

      /* Estrutura de dados : Limite de mem�ria dispon�vel */

         static long LimiteMemoria = 0 ;

      /* Estrutura de dados : Limite de n�mero de espa�os alocados */

         static long LimiteNumEspacos = 0 ;

      /* Estrutura de dados : Limite de freq��ncia estabelecido */

         static int FrequenciaMalloc ;

      /* Estrutura de dados : Contagem de NULLs gerados */

         static long ContaNULL ;

   /***** Prot�tipos de pacotes internos ao m�dulo *****/
       
      static void DesalocarEspaco( tpElemListaControle * pEspaco ) ;
       
      static tpElemListaControle * ObterOrigemElemento( void * Ponteiro ) ;
       
      static void ExibirCabecalho( char * TipoMsg , tpElemListaControle * pEspaco ) ;
       
      static void ExibirMensagem( char * Msg ) ;
       
      static int VerificarEspaco( tpElemListaControle * pEspaco ) ;

/********************** C�digo dos pacotes do m�dulo **********************/

/***************************************************************************
*
*  Fun��o: CED  &Inicializar o controlador de aloca��o de espa�os din�micos
*
*  Descri��o da fun��o
*     Esta fun��o inicializa o controlador de espa�os em mem�ria din�mica.
*     A fun��o deve ser chamada uma �nica vez, antes de qualquer outra fun��o
*     do m�dulo.
*     Sugere-se que seja chamada ao iniciar o teste do m�dulo controlado.
*     O interpretador de comandos de controle de espa�os din�micos
*     prov� o comando de teste  =inicializarespacos  que chama a presente
*     fun��o.
*
***************************************************************************/

   void CED_InicializarControlador( void )
   {

   /* Raiz de CED  &Inicializar o controlador de aloca��o de espa�os din�micos */

      TST_ASSERT( !estaInicializado ) ;

      pOrgLista            = NULL ;
      pEspacoCorr          = NULL ;
      numEspacosAlocados   = 0 ;
      TotalAlocacoes       = 0 ;
      EspacoTotalAlocado   = 0 ;
      LimiteMemoria        = 0 ;
      LimiteNumEspacos     = 0 ;
      FrequenciaMalloc     = 0 ;
      ContaNULL            = 0 ;

      ALT_Inicializar( 0 ) ;

      estaInicializado     = TRUE ;

   } /* Fim pacote: Raiz de CED  &Inicializar o controlador de aloca��o de espa�os din�micos */

/***************************************************************************
*
*  Fun��o: CED  &Alocar espa�o
*
*  Descri��o da fun��o
*     Nos m�dulos controlados esta fun��o substitui a fun��o "malloc".
*     A susbtitui��o � transparente para o m�dulo controlado,
*     basta que inclua o m�dulo de controle de acesso a espa�os de dados
*     din�micos. Ou seja, se o m�dulo de defini��o "CESPDIN.H" for
*     incluido, a presente fun��o ser� utilizada ao chamar "malloc"
*     dentro do m�dulo cliente. N�o sendo incluido "CESPDIN.H", ser�
*     chamada a fun��o "malloc" padr�o de C.
*
*     O espa�o ser� alocado com controle somente se o m�dulo de controle
*     de espa�o din�mico tiver sido inicializado, ver comando de teste
*
*         =inicializarespacos     e a correspondente fun��o.
*
*     Desta forma pode-se restringir o controle somente aos m�dulos
*     sob teste.
*
*     O valor (parte �til do espa�o alocado) � inicializado para o
*     caractere '|', permitindo a verifica��o de se o valor foi
*     corretamente inicializado ou n�o ap�s � inicializa��o na fun��o
*     cliente.
*
*     Para assegurar transpar�ncia � definida a macro CED_MALLOC
*     que providencia os argumentos linha e arquivo fonte. Estes, por sua
*     vez, permitem identificar onde o espa�o foi alocado, facilitando,
*     assim, resolver eventuais problemas de vazamento de mem�ria
*     e/ou de uso incorreto de espa�os.
*
*     A fun��o aloca um "struct" contendo diversos controles al�m do
*     espa�o �til (valor). O campo valor deste "struct" corresponde ao
*     espa�o que seria alocado pela fun��o de biblioteca "malloc".
*     O ponteiro retornado aponta para o campo valor, tornando
*     transparente o fato de se utilizar "malloc" ou esta fun��o.
*
*     A fun��o pode simular o esgotamento de mem�ria. Para isto � necess�rio
*     estabelecer um limite atrav�s de uma das fun��es a isto destinadas.
*     Por exemplo, se o limite de espa�o alocado for diferente de zero e o
*     total de espa�o j� alocado acrescido do espa�o solicitado for maior
*     do que o limite estabelecido, a fun��o n�o aloca espa�o e retorna NULL,
*     simulando falta de mem�ria.
*
*     A fun��o pode simular esgotamento aleat�rio de mem�ria. Para isto
*     � necess�rio estabelecer a freq��ncia de NULLs simulados, vela o
*     comando =limitarfrequencia em INTRPCED.C
*
*     Consulte as fun��es CED_Limitar... para detalhes das formas de
*     limite que podem ser estabelecidas.
*
***************************************************************************/

   void * CED_Malloc( size_t Tamanho ,
                      int    numLinhaFonte ,
                      char * NomeArquivoFonte )
   {

   /* Dados do bloco: Raiz de CED  &Alocar espa�o */

      tpElemListaControle * pEspaco ;

   /* Dados do bloco: Inicializar a identifica��o do c�digo fonte */

      int tamNomeArq ;

   /* Raiz de CED  &Alocar espa�o */

      TST_ASSERT( Tamanho > 0 ) ;
      TST_ASSERT( numLinhaFonte > 0 ) ;
      TST_ASSERT( NomeArquivoFonte != NULL ) ;

      /* Alocar sem controle quando n�o inicializado */

         if ( ! estaInicializado )
         {
            return malloc( Tamanho ) ;
         } /* if */

      /* Controlar limites de mem�ria */

         if ( LimiteMemoria > 0 )
         {
            if ( LimiteMemoria < EspacoTotalAlocado + ( long ) Tamanho )
            {
               TST_ExibirPrefixo( TIPO_MSG_INFO , "Simula falta de mem�ria." ) ;
               ContaNULL++ ;
               return NULL ;
            } /* if */
         } /* if */

         if ( LimiteNumEspacos > 0 )
         {
            if ( numEspacosAlocados >= LimiteNumEspacos )
            {
               TST_ExibirPrefixo( TIPO_MSG_INFO , "Limita n�mero de espa�os alocados." ) ;
               ContaNULL++ ;
               return NULL ;
            } /* if */
         } /* if */

         if ( FrequenciaMalloc > 0 )
         {
            if ( ALT_GerarFrequencia( 1 , &FrequenciaMalloc , LIMITE_FREQUENCIA ) > 0)
            {
               TST_ExibirPrefixo( TIPO_MSG_INFO , "Simulou NULL por freq��ncia." ) ;
               ContaNULL++ ;
               return NULL ;
            } /* if */
         } /* if */

      /* Alocar o espa�o solicitado */

         pEspaco = ( tpElemListaControle * )
                     malloc( sizeof( tpElemListaControle ) + Tamanho -
                             DIM_VALOR ) ;

         if ( pEspaco == NULL )
         {
            TST_ExibirPrefixo( TIPO_MSG_INFO , "Mem�ria real insuficiente." ) ;
            return NULL ;
         } /* if */


         /* Alocar o espa�o solicitado */

         pEspaco = ( tpElemListaControle * )
                     malloc( sizeof( tpElemListaControle ) + Tamanho -
                             DIM_VALOR ) ;

         if ( pEspaco == NULL )
         {
            return NULL ;
         }

         EspacoTotalAlocado += Tamanho ;

      /* Inicializar os valores de controle do espa�o */

         pEspaco->ImpressaoDigital = IMPRESSAO_DIGITAL ;
         pEspaco->tamTotal         = Tamanho + sizeof( tpElemListaControle ) ;
         pEspaco->idEspaco         = TotalAlocacoes ;
         pEspaco->idTipoValor      = CED_ID_TIPO_VALOR_NULO ;
         pEspaco->ehAtivo          = TRUE ;
         pEspaco->tamValor         = Tamanho ;
         memcpy( pEspaco->ControleAntes ,   CONTROLE  , DIM_CONTROLE ) ;
         memset( pEspaco->Valor ,           CHAR_ALOC , Tamanho ) ;
         memcpy( pEspaco->Valor + Tamanho , CONTROLE  , DIM_CONTROLE ) ;

      /* Inicializar a identifica��o do c�digo fonte */

         pEspaco->numLinhaFonte = numLinhaFonte ;

         tamNomeArq = strlen( NomeArquivoFonte ) ;
         if ( tamNomeArq >= DIM_NOME_ARQ_FONTE )
         {
            NomeArquivoFonte += ( tamNomeArq - DIM_NOME_ARQ_FONTE + 1 ) ;
         } /* if */
         strcpy( pEspaco->NomeArquivoFonte , NomeArquivoFonte ) ;

      /* Encadear na lista de espa�os alocados */

         pEspaco->pAnt      = NULL ;
         pEspaco->pProx     = pOrgLista ;
         if ( pOrgLista != NULL )
         {
            pOrgLista->pAnt = pEspaco ;
         } /* if */
         pOrgLista          = pEspaco ;

         numEspacosAlocados ++ ;
         TotalAlocacoes ++ ;

      return ( void * ) &( pEspaco->Valor ) ;

   } /* Fim pacote: Raiz de CED  &Alocar espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Desalocar espa�o
*
*  Descri��o da fun��o
*     Elimina o elemento da lista de controle. Todos os bytes s�o tornados
*     iguais a 'x'. O identificador do tipo do espa�o tamb�m
*     � destru�do, possibilitando verificar se ele ainda est� alocado.
*
*     Ao usar esta fun��o deve-se tomar o cuidado de nunca usar free para
*     desalocar um espa�o controlado por este m�dulo.
*     Ou seja, evite o uso de malloc e free em m�dulos distintos em
*     que malloc est� num m�dulo controlado e free n�o.
*     O mais seguro � ou ter todos os m�dulos sob controle, ou ter nenhum.
*
***************************************************************************/

   void CED_Free( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Desalocar espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Desalocar espa�o */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         TST_ExibirPrefixo( TIPO_MSG_INFO , "Desalocar espa�o recebeu ponteiro nulo" ) ;
         return ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         free( Ponteiro ) ;
         return ;
      } /* if */

      DesalocarEspaco( pEspaco ) ;

   } /* Fim pacote: Raiz de CED  &Desalocar espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Exibir conte�do do espa�o
*
*  Descri��o da fun��o
*     Exibe o conteudo completo de um espa�o de mem�ria.
*     A parte �til � exibida em formato hexadecimal convencional
*
***************************************************************************/

   void CED_ExibirEspaco( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Exibir conte�do do espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Exibir conte�do do espa�o */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         TST_ExibirPrefixo( TIPO_MSG_INFO , "Exibir espa�o recebeu ponteiro nulo." ) ;
         return ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return ;
      } /* if */

      ExibirCabecalho( TIPO_MSG_INFO , pEspaco ) ;

      TST_ExibirEspacoHexa( pEspaco->tamValor , pEspaco->Valor ) ;

   } /* Fim pacote: Raiz de CED  &Exibir conte�do do espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Exibir todos os espa�os
*
*  Descri��o da fun��o
*     Exibe todos os espa�os que satisfazem uma determinada regra.
*     Ver o tipo CED_tpModoExibir para uma explica��o quanto �s regras
*     de sele��o de espa�os a exibir.
*        CED_ExibirTodos     - exibe todos os espa�os
*        CED_ExibirAtivos    - exibe os espa�os marcados ativos
*        CED_ExibirInativos  - exibe os espa�os marcados n�o ativos
*
***************************************************************************/

   void CED_ExibirTodosEspacos( CED_tpModoExibir Regra )
   {

   /* Dados do bloco: Tratar modo de exibir */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Exibir todos os espa�os */

      TST_ASSERT( estaInicializado ) ;

      pEspaco = pOrgLista ;

      while ( pEspaco != NULL ) {

      /* Tratar modo de exibir */

         switch ( Regra ) {

         /* Exibir todos os espa�os */

            case CED_ExibirTodos :
            {

               CED_ExibirEspaco( &( pEspaco->Valor )) ;

               break ;

            } /* fim ativa: Exibir todos os espa�os */

         /* Exibir todos os espa�os alocados ativos */

            case CED_ExibirAtivos :
            {

               if ( pEspaco->ehAtivo )
               {
                  CED_ExibirEspaco( &( pEspaco->Valor )) ;
               } /* if */

               break ;

            } /* fim ativa: Exibir todos os espa�os alocados ativos */

         /* Exibir todos os espa�os alocados inativos */

            case CED_ExibirInativos :
            {

               if ( !( pEspaco->ehAtivo ))
               {
                  CED_ExibirEspaco( &( pEspaco->Valor )) ;
               } /* if */

               break ;

            } /* fim ativa: Exibir todos os espa�os alocados inativos */

         /* Exibir nada */

            default :
            {

               break ;

            } /* fim ativa: Exibir nada */

         } /* fim seleciona: Tratar modo de exibir */

      /* Avan�ar para o pr�ximo espa�o a exibir */

         pEspaco = pEspaco->pProx ;

      } /* fim repete: Raiz de CED  &Exibir todos os espa�os */

   } /* Fim pacote: Raiz de CED  &Exibir todos os espa�os */

/***************************************************************************
*
*  Fun��o: CED  &Definir tipo do espa�o
*
*  Descri��o da fun��o
*     Caso o tipo do espa�o ainda n�o esteja definido, atribui o tipo.
*
*     O tipo do espa�o � inicializado para CED_ID_TIPO_VALOR_NULO
*     quando da aloca��o com CED_Malloc. Posteriormente pode ser
*     alterado com a presente fun��o.
*
*     A atribui��o ser� realizada somente se uma das condi��es a seguir
*     for verdadeira:
*     - o tipo a ser atribuido � CED_ID_TIPO_VALOR_NULO
*     - ou o tipo do espa�o � CED_ID_TIPO_VALOR_NULO
*
*     Para trocar o tipo de um espa�o precisa-se primeiro torn�-lo
*     igual a CED_ID_TIPO_VALOR_NULO para, depois, atribuir o novo tipo.
*
*     Uma vez definido o tipo do espa�o, fun��es cliente podem controlar
*     a consist�ncia entre o tipo implicitamente associado ao ponteiro
*     e o tipo que o espa�o de fato cont�m.
*
*     Sugere-se que o desenvolvedor da aplica��o crie uma tabela global
*     contendo um "enum" com os nomes de todos os tipos ('typedef struct') que
*     podem ser utilizados como declaradores de espa�os din�micos.
*     Ao definir o tipo, pode-se, agora, utilizar o nome correspondente
*     ao tipo do espa�o alocado como o idTipo a ser definido. Veja
*     as explica��es no arquivo IdTiposEspaco.def no subdiret�rio Tabelas
*
*     O presente sistema de controle de tipos n�o prev� mecanismos para
*     tratamento de heran�a.
*
***************************************************************************/

   int CED_DefinirTipoEspaco( void * Ponteiro , int idTipo )
   {

   /* Dados do bloco: Raiz de CED  &Definir tipo do espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Definir tipo do espa�o */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         return FALSE ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;

      if ( pEspaco == NULL )
      {
         return FALSE ;
      } /* if */

      if ( ( idTipo == CED_ID_TIPO_VALOR_NULO )
        || ( pEspaco->idTipoValor == CED_ID_TIPO_VALOR_NULO ))
      {
         pEspaco->idTipoValor = idTipo ;
         return TRUE ;
      } /* if */

      return FALSE ;

   } /* Fim pacote: Raiz de CED  &Definir tipo do espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Marcar ativo o espa�o
*
*  Descri��o da fun��o
*     Marca determinado espa�o como ativo.
*
*     A marca ativo/n�o ativo � utilizada para apoiar o controle de
*     vazamento de mem�ria. Siga o seguinte roteiro:
*     - Marque todos os espa�os n�o ativos.
*     - A partir das �ncoras, percorra as estruturas ativas marcando
*       ativos os espa�os visitados. Este percorrimento � recursivo.
*       Retorna-se da chamada se encontrar um espa�o j� marcado ativo, ou
*       se n�o tiver para onde caminhar. Note que para poder fazer isto
*       � necess�rio conhecer-se o tipo dos dados contidos em cada um
*       dos espa�os alocados.
*     - Percorra a lista de todos os espa�os verificando se cada espa�o
*       visitado � ou n�o inativo. Os n�o ativos provavelmente correspondem
*       a vazamento de mem�ria.
*
***************************************************************************/

   void CED_MarcarEspacoAtivo( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Marcar ativo o espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Marcar ativo o espa�o */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         return ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return ;
      } /* if */

      pEspaco->ehAtivo = TRUE ;

   } /* Fim pacote: Raiz de CED  &Marcar ativo o espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Marcar n�o ativo o espa�o
*
*  Descri��o da fun��o
*     Ver CED_MarcarEspacoAtivo
*
***************************************************************************/

   void CED_MarcarEspacoNaoAtivo( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Marcar n�o ativo o espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Marcar n�o ativo o espa�o */

      TST_ASSERT( estaInicializado ) ;
         
      if ( Ponteiro == NULL )
      {
         return ;
      } /* if */
         
      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return ;
      } /* if */
         
      pEspaco->ehAtivo = FALSE ;

   } /* Fim pacote: Raiz de CED  &Marcar n�o ativo o espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Marcar n�o ativos todos os espa�os
*
*  Descri��o da fun��o
*     Percorre todos os espa�os alocados marcando-os inativos.
*
*     Veja: CED_MarcarEspacoAtivo
*
***************************************************************************/

   void CED_MarcarTodosEspacosInativos( )
   {

   /* Dados do bloco: Raiz de CED  &Marcar n�o ativos todos os espa�os */

      tpElemListaControle * pEspaco ;
         

   /* Raiz de CED  &Marcar n�o ativos todos os espa�os */

      TST_ASSERT( estaInicializado ) ;
         
      pEspaco = pOrgLista ;
      while ( pEspaco != NULL )
      {
         pEspaco->ehAtivo = FALSE ;
         pEspaco = pEspaco->pProx ;
      } /* while */

   } /* Fim pacote: Raiz de CED  &Marcar n�o ativos todos os espa�os */

/***************************************************************************
*
*  Fun��o: CED  &Limitar mem�ria dispon�vel
*
*  Descri��o da fun��o
*     Estabelece um limite de mem�ria dispon�vel. Este limite afeta
*     o comportamento da fun��o CED_Malloc.
*
*     Embora a fun��o possa ser chamada a qualquer momento, recomenda-se
*     que seja chamada antes do primeiro caso de teste capaz de alocar
*     espa�o de mem�ria din�mica por interm�dio da fun��o CED_Malloc
*     contida nesse m�dulo.
*
***************************************************************************/

   void CED_LimitarMemoriaDisponivel( long NovoLimiteMemoria )
   {

   /* Raiz de CED  &Limitar mem�ria dispon�vel */

      TST_ASSERT( estaInicializado ) ;
      TST_ASSERT( NovoLimiteMemoria >= 0 ) ;

      LimiteMemoria = NovoLimiteMemoria ;

   } /* Fim pacote: Raiz de CED  &Limitar mem�ria dispon�vel */

/***************************************************************************
*
*  Fun��o: CED  &Limitar n�mero de espa�os alocados
*
*  Descri��o da fun��o
*     Limita o n�mero de espa�os alocados. Esta fun��o afeta o comportamento
*     de CED_Malloc, estabelecendo um limite para o n�mero total de
*     espa�os alocados em um dado momento.
*
*     Embora a fun��o possa ser chamada a qualquer momento, recomenda-se
*     que seja chamada antes do primeiro caso de teste que provoque
*     aloca��o de espa�o de mem�ria.
*
***************************************************************************/

   void CED_LimitarNumeroTotalEspacos( long numTotalEspacos )
   {

   /* Raiz de CED  &Limitar n�mero de espa�os alocados */

      TST_ASSERT( estaInicializado ) ;
      TST_ASSERT( numTotalEspacos >= 0 ) ;

      LimiteNumEspacos = numTotalEspacos ;

   } /* Fim pacote: Raiz de CED  &Limitar n�mero de espa�os alocados */

/***************************************************************************
*
*  Fun��o: CED  &Limitar com freq��ncia de gera��o de NULL
*
*  Descri��o da fun��o
*     Esta fun��o estabelece a freq��ncia com a qual n�o dever� ser
*     gerado NULL, o complemento a esta freq��ncia � a frequ��ncia com
*     a qual dever� ser retornado NULL (n�o aloca) independentemente da
*     exist�ncia ou n�o de mem�ria dispon�vel.
*     A fun��o visa apoiar a verifica��o de se o comportamento de um
*     programa leva em conta os retornos de falta de mem�ria.
*
***************************************************************************/

   int CED_LimitarFrequencia( int Frequencia )
   {

   /* Raiz de CED  &Limitar com freq��ncia de gera��o de NULL */

      TST_ASSERT( estaInicializado ) ;

      if ( ( Frequencia <  LIMITE_INF_FREQUENCIA )
        || ( Frequencia >= LIMITE_SUP_FREQUENCIA ))
      {
         return FALSE ;
      } /* if */

      FrequenciaMalloc = Frequencia ;

      return TRUE ;

   } /* Fim pacote: Raiz de CED  &Limitar com freq��ncia de gera��o de NULL */

/***************************************************************************
*
*  Fun��o: CED  &Limitar, eliminar limites
*
*  Descri��o da fun��o
*     Esta fun��o elimina todos os limites de mem�ria por ventura em vigor.
*
***************************************************************************/

   void CED_EliminarLimites( )
   {

   /* Raiz de CED  &Limitar, eliminar limites */

      TST_ASSERT( estaInicializado ) ;

      LimiteMemoria        = 0 ;
      LimiteNumEspacos     = 0 ;
      FrequenciaMalloc     = 0 ;

   } /* Fim pacote: Raiz de CED  &Limitar, eliminar limites */

/***************************************************************************
*
*  Fun��o: CED  &Obter o tipo do espa�o de dados
*
*  Descri��o da fun��o
*     Retorna o identificador do tipo do espa�o apontado.
*
***************************************************************************/

   int CED_ObterTipoEspaco( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Obter o tipo do espa�o de dados */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Obter o tipo do espa�o de dados */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         return CED_PONTEIRO_NULO ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return CED_ID_TIPO_ILEGAL ;
      } /* if */

      return pEspaco->idTipoValor ;

   } /* Fim pacote: Raiz de CED  &Obter o tipo do espa�o de dados */

/***************************************************************************
*
*  Fun��o: CED  &Obter tamanho do valor contido no espa�o
*
*  Descri��o da fun��o
*     Retorna o n�mero de bytes �teis do espa�o
*
***************************************************************************/

   int CED_ObterTamanhoValor( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Obter tamanho do valor contido no espa�o */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Obter tamanho do valor contido no espa�o */

      TST_ASSERT( estaInicializado ) ;
         
      if ( Ponteiro == NULL )
      {
         return -1 ;
      } /* if */
         
      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return -1 ;
      } /* if */

      return pEspaco->tamValor ;

   } /* Fim pacote: Raiz de CED  &Obter tamanho do valor contido no espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Obter n�mero de espa�os alocados
*
*  Descri��o da fun��o
*     Retorna o n�mero de elementos da lista de espa�os alocados
*
***************************************************************************/

   int CED_ObterNumeroEspacosAlocados( )
   {

   /* Raiz de CED  &Obter n�mero de espa�os alocados */

      TST_ASSERT( estaInicializado ) ;

      return numEspacosAlocados ;

   } /* Fim pacote: Raiz de CED  &Obter n�mero de espa�os alocados */

/***************************************************************************
*
*  Fun��o: CED  &Obter total de espa�os alocados
*
*  Descri��o da fun��o
*     Retorna o n�mero total de vezes que foi chamada a fun��o
*     CED_Malloc.
*
***************************************************************************/

   int CED_ObterTotalAlocacoes( )
   {

   /* Raiz de CED  &Obter total de espa�os alocados */

      TST_ASSERT( estaInicializado ) ;

      return TotalAlocacoes ;

   } /* Fim pacote: Raiz de CED  &Obter total de espa�os alocados */

/***************************************************************************
*
*  Fun��o: CED  &Obter n�mero total de NULLs gerados
*
*  Descri��o da fun��o
*     Retorna o n�mero de NULLs retornados em virtude de limita��es impostas.
*     Veja as funcoes CED_LimitarXXX
*
***************************************************************************/

   long CED_ObterNumNULL( void )
   {

   /* Raiz de CED  &Obter n�mero total de NULLs gerados */

      TST_ASSERT( estaInicializado ) ;

      return ContaNULL ;

   } /* Fim pacote: Raiz de CED  &Obter n�mero total de NULLs gerados */

/***************************************************************************
*
*  Fun��o: CED  &Obter n�mero de espa�os segundo regra
*
*  Descri��o da fun��o
*     Obt�m o n�mero de elementos contidos na lista de espa�os alocados,
*     segundo a regra:
*        CED_ExibirTodos     - exibe todos os espa�os
*        CED_ExibirAtivos    - exibe os espa�os marcados ativos
*        CED_ExibirInativos  - exibe os espa�os marcados n�o ativos
*
***************************************************************************/

   int CED_ObterNumeroEspacos( CED_tpModoExibir Regra )
   {

   /* Dados do bloco: Raiz de CED  &Obter n�mero de espa�os segundo regra */

      tpElemListaControle * pEspaco ;
      int Conta = 0 ;

   /* Raiz de CED  &Obter n�mero de espa�os segundo regra */

      TST_ASSERT( estaInicializado ) ;
               
      pEspaco = pOrgLista ;

      while ( pEspaco != NULL )
      {
         switch ( Regra )
         {
            case CED_ExibirTodos :
               Conta ++ ;
               break ;

            case CED_ExibirAtivos :
               if ( pEspaco->ehAtivo )
               {
                  Conta ++ ;
               } /* if */
               break ;

            case CED_ExibirInativos :
               if ( !( pEspaco->ehAtivo ))
               {
                  Conta ++ ;
               } /* if */
               break ;

            default:
               break ;

         } /* fim switch */

         pEspaco = pEspaco->pProx ;
      } /* while */

      return Conta ;

   } /* Fim pacote: Raiz de CED  &Obter n�mero de espa�os segundo regra */

/***************************************************************************
*
*  Fun��o: CED  &Obter espa�o total alocado
*
*  Descri��o da fun��o
*     Obt�m o total, em bytes, de espa�os �teis alocados.
*
***************************************************************************/

   long CDE_ObterEspacoTotalAlocado( )
   {

   /* Raiz de CED  &Obter espa�o total alocado */

      TST_ASSERT( estaInicializado ) ;

      return EspacoTotalAlocado ;

   } /* Fim pacote: Raiz de CED  &Obter espa�o total alocado */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: iniciar iterador de espa�os
*
*  Descri��o da fun��o
*     Inicia o iterador com o primeiro espa�o da lista de controle de espa�os
*     din�micos.
*
*     O iterador pode ser utilizado para verificar a integridade dos
*     espa�os de dados sob controle do m�dulo. Para isto a fun��o de
*     verifica��o de espa�os deve percorrer todos os espa�os alocados
*     disparando o controle individual para cada um deles.
*     Pode-se tamb�m simular um garbage collector, percorrendo todos
*     os espa�os e marca-los inativos. Depois percorrer, a partir
*     das ancoras, todos os espa�os adjacentes, marcando ativos os
*     espa�os visitados. Evidentemente se j� estiver marcado ativo
*     retorna-se para tratar do pr�ximo estado pendente na pilha.
*     Ao final, os espa�os que ainda estiverem marcados inativos
*     correspondem a vazamento de mem�ria.
*
***************************************************************************/

   void CED_InicializarIteradorEspacos( )
   {

   /* Raiz de CED  &Iterador: iniciar iterador de espa�os */

      TST_ASSERT( estaInicializado ) ;

      pEspacoCorr = pOrgLista ;

   } /* Fim pacote: Raiz de CED  &Iterador: iniciar iterador de espa�os */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: avan�ar para o pr�ximo espa�o
*
*  Descri��o da fun��o
*     Avan�a o iterador para o pr�ximo espa�o.
*
***************************************************************************/

   int CED_AvancarProximoEspaco( )
   {

   /* Raiz de CED  &Iterador: avan�ar para o pr�ximo espa�o */

      TST_ASSERT( estaInicializado ) ;

      if ( pEspacoCorr == NULL )
      {
         return FALSE ;
      } /* if */

      pEspacoCorr = pEspacoCorr->pProx ;

      if ( pEspacoCorr != NULL )
      {
         return TRUE ;
      } else {
         return FALSE ;
      } /* if */

   } /* Fim pacote: Raiz de CED  &Iterador: avan�ar para o pr�ximo espa�o */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: obter refer�ncia para o espa�o corrente
*
*  Descri��o da fun��o
*     Obt�m a refer�ncia para a parte �til do espa�o corrente.
*
***************************************************************************/

   void * CED_ObterPonteiroEspacoCorrente( )
   {

   /* Raiz de CED  &Iterador: obter refer�ncia para o espa�o corrente */

      TST_ASSERT( estaInicializado ) ;

      if ( pEspacoCorr == NULL )
      {
         return NULL ;
      } /* if */

      return ( void * ) &( pEspacoCorr->Valor ) ;

   } /* Fim pacote: Raiz de CED  &Iterador: obter refer�ncia para o espa�o corrente */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: existe espa�o corrente
*
*  Descri��o da fun��o
*     Verifica se o iterador referencia um espa�o corrente.
*     Caso retorne FALSE, o iterador chegou ao final da lista de espa�os
*     ou, ent�o, ainda n�o foi inicializado.
*
***************************************************************************/

   int CED_ExisteEspacoCorrente( )
   {

   /* Raiz de CED  &Iterador: existe espa�o corrente */

      TST_ASSERT( estaInicializado ) ;

      if ( pEspacoCorr == NULL )
      {
         return FALSE ;
      } /* if */

      return TRUE ;

   } /* Fim pacote: Raiz de CED  &Iterador: existe espa�o corrente */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: terminar iterador
*
*  Descri��o da fun��o
*     Desativa o iterador. A refer�ncia para o esp�co corrente passa
*     a ser NULL.
*
***************************************************************************/

   void CED_TerminarIteradorEspacos( )
   {

   /* Raiz de CED  &Iterador: terminar iterador */

      pEspacoCorr = NULL ;

   } /* Fim pacote: Raiz de CED  &Iterador: terminar iterador */

/***************************************************************************
*
*  Fun��o: CED  &Iterador: excluir espa�o corrente
*
*  Descri��o da fun��o
*     Elimina o espa�o corrente da lista de espa�os controlados.
*     O espa�o � liberado tornando ilegais todos os acessos realizados
*     por fun��es do usu�rio.
*     Na realidade esta fun��o corresponde a um CED_Free.
*     Utilize esta fun��o somente para para eliminar espa�os sabidamente
*     inativos, ou para deturpar ao testar rea��es a anomalias em estruturas
*     de dados.
*
***************************************************************************/

   void CED_ExcluirEspacoCorrente( )
   {

   /* Dados do bloco: Raiz de CED  &Iterador: excluir espa�o corrente */

      tpElemListaControle * pDesaloca ;

   /* Raiz de CED  &Iterador: excluir espa�o corrente */

      TST_ASSERT( estaInicializado ) ;

      if ( pEspacoCorr != NULL )
      {
         pDesaloca   = pEspacoCorr ;
         pEspacoCorr = pEspacoCorr->pProx ;
         DesalocarEspaco( pDesaloca ) ;
      } /* if */

   } /* Fim pacote: Raiz de CED  &Iterador: excluir espa�o corrente */

/***************************************************************************
*
*  Fun��o: CED  &Verificar se espa�o � ativo
*
*  Descri��o da fun��o
*     Retorna TRUE se o espa�o referenciado est� marcado ativo
*
***************************************************************************/

   int CED_EhEspacoAtivo( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  &Verificar se espa�o � ativo */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Verificar se espa�o � ativo */

      TST_ASSERT( estaInicializado ) ;

      if ( Ponteiro == NULL )
      {
         return FALSE ;
      } /* if */

      pEspaco = ObterOrigemElemento( Ponteiro ) ;
      if ( pEspaco == NULL )
      {
         return FALSE ;
      } /* if */

      return pEspaco->ehAtivo ;

   } /* Fim pacote: Raiz de CED  &Verificar se espa�o � ativo */

/***************************************************************************
*
*  Fun��o: CED  &Verificar a integridade de um espa�o de dados
*
*  Descri��o da fun��o
*     Verifica a integridade de um determinado espa�o de dados.
*     Retorna o n�mero de falhas encontradas.
*     Esta fun��o pode levar a um cancelamento do programa, caso o ponteiro
*     fornecido esteja errado.
*
***************************************************************************/

   int CED_VerificarEspaco( void * Ponteiro ,
                            int ( * pVerificarValor )( void * pValor ))
   {

   /* Dados do bloco: Raiz de CED  &Verificar a integridade de um espa�o de dados */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  &Verificar a integridade de um espa�o de dados */

      TST_ASSERT( estaInicializado ) ;

      /* Verificar as refer�ncias para o espa�o */

         if ( Ponteiro == NULL )
         {
            return TRUE ;
         } /* if */

         pEspaco = ObterOrigemElemento( Ponteiro ) ;

         if ( pEspaco == NULL )
         {
            return FALSE ;
         } /* if */

      /* Verificar o conte�do do espa�o */

         if ( pVerificarValor != NULL )
         {
            if ( pVerificarValor( pEspaco->Valor ) == 0 )
            {
               ExibirMensagem( "Valor contido no espa�o est� incorreto." ) ;
               ExibirCabecalho( TIPO_MSG_VAZIA , pEspaco ) ;
               fprintf( TST_ObterArqLog( ) , "\n" ) ;
               return FALSE ;
            } /* if */
         } /* if */

      return TRUE ;

   } /* Fim pacote: Raiz de CED  &Verificar a integridade de um espa�o de dados */

/***************************************************************************
*
*  Fun��o: CED  &Verificar todos os espa�os alocados
*
*  Descri��o da fun��o
*     Percorre toda a lista, verificando um a um os espa�os.
*     Caso um ou mais espa�os estejam errados, pode ocorrer
*     cancelamento da execu��o em virtude de acesso a mem�ria ilegal.
*
***************************************************************************/

   int CED_VerificarTudo( int ( * pVerificarValor )( void * pValor ))
   {

   /* Dados do bloco: Raiz de CED  &Verificar todos os espa�os alocados */

      tpElemListaControle * pEspaco ;

      int ContaErro   = 0 ,
          ContaEspaco = 0 ;

   /* Raiz de CED  &Verificar todos os espa�os alocados */

      TST_ASSERT( estaInicializado ) ;

      /* Verificar toda a lista de espa�os */

         pEspaco = pOrgLista ;
         while ( pEspaco != NULL )
         {
            ContaEspaco ++ ;
            if ( ! CED_VerificarEspaco( pEspaco->Valor , pVerificarValor ))
            {
               ContaErro ++ ;
            } /* if */
            pEspaco = pEspaco->pProx ;
         } /* while */

      /* Verificar n�mero de espa�os alocados */

         if ( ContaEspaco != numEspacosAlocados )
         {
            ExibirMensagem( "N�mero de espa�os alocados errado." ) ;
            ContaErro ++ ;
         } /* if */

         if ( numEspacosAlocados > TotalAlocacoes )
         {
            ExibirMensagem( "N�mero de espa�os alocados maior do que total alocado." ) ;
            ContaErro ++ ;
         } /* if */

      return ContaErro == 0 ;

   } /* Fim pacote: Raiz de CED  &Verificar todos os espa�os alocados */

/***************************************************************************
*
*  Fun��o: CED  -Desalocar espa�o dado
*
*  Descri��o da fun��o
*
***************************************************************************/

   void DesalocarEspaco( tpElemListaControle * pEspaco )
   {

   /* Raiz de CED  -Desalocar espa�o dado */

      /* Desalocar o espa�o apontado */

         if ( pEspaco->pProx != NULL )
         {
            pEspaco->pProx->pAnt  = pEspaco->pAnt ;
         } /* if */

         if ( pEspaco->pAnt != NULL )
         {
            pEspaco->pAnt->pProx = pEspaco->pProx ;
         } else {
            pOrgLista = pEspaco->pProx ;
         } /* if */

         numEspacosAlocados -- ;
         EspacoTotalAlocado -= pEspaco->tamValor ;

      /* Limpar o espa�o a liberar */


         memset( pEspaco->ControleAntes , CHAR_DESALOC ,
                 pEspaco->tamValor + DIM_CONTROLE ) ;

         memcpy( pEspaco->ControleAntes + pEspaco->tamValor + DIM_CONTROLE ,
                 CONTROLE_ERRO , DIM_CONTROLE ) ;

         pEspaco->idTipoValor = CED_ID_TIPO_ILEGAL ;
         free( pEspaco ) ;

   } /* Fim pacote: Raiz de CED  -Desalocar espa�o dado */

/***************************************************************************
*
*  Fun��o: CED  -Obter ponteiro para o elemento da lista de espa�os ativos
*
*  Descri��o da fun��o
*     Retorna o ponteiro para o in�cio do elemento da lista de espa�os
*     alocados, no entanto sem controlar a integridade do esp�co.
*
***************************************************************************/

   tpElemListaControle * ObterOrigemElemento( void * Ponteiro )
   {

   /* Dados do bloco: Raiz de CED  -Obter ponteiro para o elemento da lista de espa�os ativos */

      tpElemListaControle * pEspaco ;

   /* Raiz de CED  -Obter ponteiro para o elemento da lista de espa�os ativos */

      TST_ASSERT( Ponteiro != NULL ) ;

      pEspaco = ( tpElemListaControle * )
                  ((( char * ) Ponteiro ) - ( sizeof( tpElemListaControle ) -
                     DIM_VALOR - DIM_CONTROLE )) ;

      if ( pEspaco->ImpressaoDigital != IMPRESSAO_DIGITAL )
      {
         return NULL ;
      } /* if */


      if ( VerificarEspaco( pEspaco ))
      {
         ExibirMensagem(  "Espa�o de dados est� corrompido." ) ;

         ExibirCabecalho( TIPO_MSG_VAZIA , pEspaco ) ;

         fprintf( TST_ObterArqLog( ) , "\n" ) ;

         return NULL ;
      } /* if */

      return pEspaco ;

   } /* Fim pacote: Raiz de CED  -Obter ponteiro para o elemento da lista de espa�os ativos */

/***************************************************************************
*
*  Fun��o: CED  -Exibir cabecalho do espaco
*
*  Descri��o da fun��o
*     Exibe todos os atributos, exceto o valor, contido no espa�o.
*
***************************************************************************/

   void ExibirCabecalho( char * TipoMsg , tpElemListaControle * pEspaco )
   {

   /* Dados do bloco: Raiz de CED  -Exibir cabecalho do espaco */

      FILE * pArqLog ;

      int i ;

   /* Dados do bloco: Exibir ponteiro do espa�o */

      char * pVal  ;
      char * pChar ;

   /* Raiz de CED  -Exibir cabecalho do espaco */

      pArqLog = TST_ObterArqLog( ) ;

      fprintf( pArqLog , "\n\n%s  Espa�o de dados, id: %5d" ,
                  TipoMsg , pEspaco->idEspaco ) ;

      /* Exibir ponteiro do espa�o */

         pVal  = pEspaco->Valor ;

         fprintf( pArqLog , "  Ponteiro valor: %p imagem:" , pVal ) ;

         pChar = ( char * ) &pVal ;
         for( i = 0 ; i < DIM_PONTEIRO ; i++ )
         {
            fprintf( pArqLog , " %02X" , pChar[ i ] ) ;
         } /* for */

      /* Exibir identifica��o do espa�o antecessor */

         if ( pEspaco->pAnt != NULL  )
         {
            fprintf( pArqLog , "  Id ant: %5d" , pEspaco->pAnt->idEspaco ) ;
         } else {
            fprintf( pArqLog , "  Origem" ) ;
         } /* if */

      /* Exibir identifica��o do espa�o sucessor */

         if ( pEspaco->pProx != NULL )
         {
            fprintf( pArqLog , "  Id suc: %5d" , pEspaco->pProx->idEspaco ) ;
         } else {
            fprintf( pArqLog , "  Final" ) ;
         } /* if */

      /* Exibir o identificador do tipo do valor */

         if ( pEspaco->idTipoValor != CED_ID_TIPO_VALOR_NULO )
         {
            fprintf( pArqLog , "\n     Id tipo valor: %d" , pEspaco->idTipoValor ) ;
         } else {
            fprintf( pArqLog , "\n     Tipo indefinido" ) ;
         } /* if */

      /* Tamanho �til do valor alocado */

         fprintf( pArqLog , "  Tamanho: %d" , pEspaco->tamValor ) ;

      /* Exibir flag ativo */

         if ( pEspaco->ehAtivo )
         {
            fprintf( pArqLog , "  Ativo" ) ;
         } else {
            fprintf( pArqLog , "  N�o ativo" ) ;
         } /* if */

      /* Exibir n�mero da linha de c�digo fonte onde foi alocado */

         fprintf( pArqLog , "  Linha onde alocou: %d" , pEspaco->numLinhaFonte ) ;

      /* Exibir nome do arquivo onde alocou */

         fprintf( pArqLog , "  Arquivo: %s" , pEspaco->NomeArquivoFonte ) ;

      /* Exibir controle antes */

         if ( memcmp( pEspaco->ControleAntes , CONTROLE , DIM_CONTROLE ) == 0 )
         {
            fprintf( pArqLog , "\n     Controle antes OK" ) ;
         } else {
            fprintf( pArqLog , "\n     Controle antes errado:" ) ;
            for( i = 0 ; i < DIM_CONTROLE ; i++ )
            {
               fprintf( pArqLog , " %02X" , pEspaco->ControleAntes[ i ] ) ;
            } /* for */
         } /* if */

      /* Exibir controle ap�s */

         if ( memcmp( pEspaco->ControleApos - DIM_VALOR + pEspaco->tamValor ,
                      CONTROLE , DIM_CONTROLE ) == 0 )
         {
            fprintf( pArqLog , "  Controle ap�s OK" ) ;
         } else {
            fprintf( pArqLog , "  Controle ap�s errado:" ) ;
            for( i = 0 ; i < DIM_CONTROLE ; i++ )
            {
               fprintf( pArqLog , " %02X" , pEspaco->ControleApos[ i ] ) ;
            } /* for */
         } /* if */

   } /* Fim pacote: Raiz de CED  -Exibir cabecalho do espaco */

/***************************************************************************
*
*  Fun��o: CED  -Exibir mensagem de erro
*
*  Descri��o da fun��o
*     Exibe uma mensagem de erro ou advert�ncia no formato padr�o
*
***************************************************************************/

   void ExibirMensagem( char * Msg )
   {

   /* Raiz de CED  -Exibir mensagem de erro */

      TST_ContarFalhas( ) ;
      TST_ExibirPrefixo( TIPO_MSG_ERRO , Msg ) ;

   } /* Fim pacote: Raiz de CED  -Exibir mensagem de erro */

/***************************************************************************
*
*  Fun��o: CED  -Verificar integridade de determinado espa�o
*
*  Descri��o da fun��o
*     Controla a integridade ap�s conhecer o ponteiro para a origem
*     do espa�o. N�o controla o campo valor.
*
***************************************************************************/

   int VerificarEspaco( tpElemListaControle * pEspaco )
   {

   /* Dados do bloco: Raiz de CED  -Verificar integridade de determinado espa�o */

      int ContaErro = 0 ;

   /* Raiz de CED  -Verificar integridade de determinado espa�o */

      /* Verificar id espa�o */

         if ( ( pEspaco->idEspaco   < 0 )
           || ( pEspaco->idEspaco   > TotalAlocacoes )
           || ( numEspacosAlocados  > TotalAlocacoes ))
         {
            ContaErro ++ ;
            ExibirMensagem( "Identifica��o do espa�o est� errada." ) ;
         } /* if */

      /* Verificar encadeamento origem da lista */

         if ( pOrgLista != NULL )
         {
            if ( pOrgLista->pAnt != NULL )
            {
               ContaErro ++ ;
               ExibirMensagem( "Origem da lista de espa�os est� errada." ) ;
            } /* if */
         } else {
            ContaErro ++ ;
            ExibirMensagem( "Origem da lista n�o vazia � NULL." ) ;
            pOrgLista = pEspaco ;
         } /* if */

      /* Verificar encadeamento antes */

         if ( pEspaco != pOrgLista )
         {
            if ( pEspaco->pAnt != NULL )
            {
               if ( pEspaco->pAnt->pProx != pEspaco )
               {
                  ContaErro ++ ;
                  ExibirMensagem( "Encadeamento antes est� errado." ) ;
                  pEspaco->pAnt = NULL ;
               } /* if */
            } else {
               ContaErro ++ ;
               ExibirMensagem( "Esp�co anterior == NULL, mas n�o � a origem da lista de espa�os." ) ;
            } /* if */
         } else
         {
            if ( pEspaco->pAnt != NULL )
            {
               ContaErro ++ ;
               ExibirMensagem( "Espa�o origem da lista est� errada." ) ;
               pEspaco->pAnt = NULL ;
            } /* if */
         } /* if */

      /* Verificar encadeamento ap�s */

         if ( pEspaco->pProx != NULL )
         {
            if ( pEspaco->pProx->pAnt != pEspaco )
            {
               ContaErro ++ ;
               ExibirMensagem( "Encadeamento ap�s est� errado." ) ;
               pEspaco->pProx = NULL ;
            } /* if */
         } /* if */

      /* Verificar tamanho */

         if ( ( pEspaco->tamTotal != pEspaco->tamValor +
                          ( int ) sizeof( tpElemListaControle ))
           || ( pEspaco->tamValor <= 0 ))
         {
            ContaErro ++ ;
            ExibirMensagem( "Tamanho do espa�o est� errado." ) ;
         } /* if */

      /* Verificar controle antes */

         if ( memcmp( pEspaco->ControleAntes , CONTROLE , DIM_CONTROLE ) != 0 )
         {
            ContaErro ++ ;
            ExibirMensagem( "Controle de extravas�o antes est� errado." ) ;
         } /* if */

      /* Verificar controle ap�s */

         if ( memcmp( pEspaco->ControleApos - DIM_VALOR + pEspaco->tamValor ,
                      CONTROLE , DIM_CONTROLE ) != 0 )
         {
            ContaErro ++ ;
            ExibirMensagem( "Controle de extravas�o ap�s est� errado." ) ;
         } /* if */

      return ContaErro ;

   } /* Fim pacote: Raiz de CED  -Verificar integridade de determinado espa�o */

/********** Fim do m�dulo: CED  Controlador de espa�os de dados alocados dinamicamente **********/
