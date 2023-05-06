#!/bin/bash

if [ $# != 4 ]; then
	echo -e "\n脚本需要四个参数"
	echo -e "  参数1: 原始编码格式\n  参数2：目标编码格式\n  参数3: 待转码的文件目录\n  参数4: 转码后的文件目录\n"
	echo -e "示例: icon.sh  from_encoding  to_encoding  in_dir  out_dir\n"
	exit
else
	FROM_ENCODING=$1
	TO_ENCODING=$2
	IN_DIR=$3
	OUT_DIR=$4
fi

#参数：两个参数， 输入目录和输出目录
#功能：使输出目录中目录树与输入目录相同
create_outDirTree() {
	in_dir=$1
	out_dir=$2

	for dir in $(find $in_dir -type d); do
		sub_dir=$(echo $dir | awk -F "$in_dir" '{print $2}')
		new_dir=${out_dir}/$sub_dir
		mkdir -p $new_dir
	done
}

#参数： 四个参数， 原始编码、目标编码、原始文件、目标文件
#功能： 实现文件的编码转换
iconv_file() {
	iconv -f $1 -t $2 $3 -o $4 1>iconvLog 2>&1

	errString=$(cat iconvLog | grep "illegal input sequence")

	if [ -z "$errString" ]; then
		:
	else
		echo -e "$3\n   $errString"
	fi

	rm -rf iconvLog
}

#功能： 将源目录中的所有文件进行编码转换， 输出到另一目录， 源目录文件保持不变
main() {
	create_outDirTree $IN_DIR $OUT_DIR

	for f in $(find $IN_DIR -type f); do
		fname=$(echo $f | awk -F "$IN_DIR" '{print $2}')
		if echo "$fname" | grep -q -i -E '\.c$|\.txt$'; then
			out_fname=$OUT_DIR/$fname
			iconv_file $FROM_ENCODING $TO_ENCODING $f $out_fname
		else
			out_fname=$OUT_DIR/$fname
			cp $f $out_fname
		fi
	done

}

#主程序
main
