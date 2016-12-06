#/bin/sh

#testy jednotlivých modulů
dirs="lex synt sem int other"
for dir in $dirs
do
	echo ""
	echo "====================Testing module $dir===================="
	echo ""
	tests=`find tests/$dir | grep .ifj16$ | sort`
	for i in $tests
	do
		# Spustí program
		echo "Runing $i"
		./ifj16_team54 $i <$i.in >$i.out

		# Vyhodnotí návratovou hodnotu
		ret=$?
		ref=`cat $i.ret`
		if [ $ret -eq $ref ]
		then
			echo "Returned $ret - OK"
		else
			echo "Returned $ret instead of $ref!!!"
		fi

		# Vyhodnotí výstup
		if diff $i.out $i.output >/dev/null
		then
			echo "$i: Output OK"
			rm "$i.out"
		else
			echo "$i: Incorrect output data"
			echo "$i.out saved"
		fi

		echo ""
	done
done
