
for f in $1/*.json
do
	if [[ ! ${f%.*} =~ .*"-tune" ]]; then
		/opt/mongodb-osx-x86_64-3.0.0/bin/mongoimport --db skweltchdev --collection machines $f
	fi
done
