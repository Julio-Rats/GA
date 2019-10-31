#Verifica argumentos
if [ $# -lt 2 ]; then
	 echo -e "\nSintaxe de uso do script:\n\n\tsh $0 [Input] [Nº Replicações]\n"
	 echo -e "\nArgumentos:\n\n"
	 echo -e "[Input]: Arquivo DBC para simulação ou otimização."
	 echo -e "[Nº Replicações]: Numero de vezes que o script ira executar o processo principal.\n"
	 exit
fi

# Quantidade de repeticoes desejada
NUM_REPETICOES=$2

# Nome dos arquivos de entrada. Ajuste para customizar o script
Path_Result="./results/"

# Executa a aplicacao a quantidade de repeticoes especificada
CONTADOR=1
until [ $CONTADOR -gt $NUM_REPETICOES ];
do
	data=$(date +%s)
	echo -e "\nExecução Nº: $CONTADOR de $NUM_REPETICOES Execuções \t Time Start = $data"
	echo

	./ga.bin $1 $Path_Result$data-$1

	CONTADOR=$(($CONTADOR+1))
done
