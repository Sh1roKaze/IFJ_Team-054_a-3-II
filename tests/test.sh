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
		err=0
		./ifj16_team54 $i <$i.in >$i.out 2>$i.err

		# Vyhodnotí návratovou hodnotu
		ret=$?
		ref=`cat $i.ret`
		if [ $ret -eq $ref ]
		then
			res="Returned $ret - OK"
		else
			res="Returned $ret - should return $ref!!!"
			err=1
		fi

		# Vyhodnotí výstup
		if diff $i.out $i.output >/dev/null
		then
			out="Output OK"
			rm "$i.out"
		else
			out="Incorrect output data.\n$i.out saved."
			err=1
		fi
		
		# Vypíše výsledek
		if [ $err -eq 0 ]
		then
			echo "$i - OK"
		else
			echo ""
			echo $i
			echo $res
			echo $out
			cat $i.err
			echo ""
		fi
		rm $i.err
	done
done
