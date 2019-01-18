/* test */

var pikachu = `<paml>

	<info>
		author=Nintendo
		dateofcreation=2013-05-31
		license=Recreated_by_Joe_DF
		xpixels=19
		ypixels=20
		sizexpixels=10
		sizeypixels=10
		bgcolor=#0055AA ;blue
	</info>

	<defcolor>
		b=black ;or o=b000000
		g=#555555 ;Dark grey
		w=white
		y=yellow
		x=#D3D300
		r=#FF3E3E
	</defcolor>

	<drawpixels>
		 , , , , ,b,b, , , , , , , , , ,b, , ,
		 , , , ,b,g,b, , , , , , , , ,b,y,b, ,
		 , , , ,b,y,b, , , , , , , ,b,y,x,x,b,
		 , , ,b,y,y,b, , , , ,b,b,b,y,x,x,x,b,
		 , , ,b,y,x,b, , ,b,b,g,g,b,x,x,x,b, ,
		 , ,b,y,y,y,y,b,b,y,y,g,b,x,x,x,b, , ,
		 ,b,y,y,y,y,y,y,y,y,y,b,b,x,x,b, , , ,
		b,w,y,y,y,y,y,y,y,y,x,b, ,b,x,x,b, , ,
		b,b,y,y,y,y,y,y,y,y,y,b, , ,b,x,b, , ,
		b,y,y,y,y,w,b,y,y,y,y,x,b,b,x,b, , , ,
		 ,b,y,y,y,b,b,r,r,y,x,x,b,b,b, , , , ,
		 , ,b,x,y,y,y,r,x,x,x,b,b,b, , , , , ,
		 ,b,y,x,x,x,x,x,x,x,x,x,x,b, , , , , ,
		 , ,b,b,y,y,y,y,y,b,x,x,b,b, , , , , ,
		 , , ,b,x,y,y,y,b,y,x,x,x,b, , , , , ,
		 , ,b,x,b,x,x,x,x,b,x,x,x,b, , , , , ,
		 , ,b,b,b,b,b,x,x,x,x,x,b, , , , , , ,
		 , , , , , , ,b,b,b,x,b,b, , , , , , ,
		 , , , , , , , ,b,y,y,x,b, , , , , , ,
		 , , , , , , , , ,b,b,b, , , , , , , ,
	</drawpixels>

</paml>`;


var parser = new DOMParser();
var doc = parser.parseFromString(sxx, "application/xml");
var paml = doc.getElementsByTagName('paml')[0];

var rInfo = paml.getElementsByTagName('info')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
var rDefs = paml.getElementsByTagName('defcolor')[0].textContent.trim().split('\n');
var rDPix = paml.getElementsByTagName('drawpixels')[0].textContent.trim().split(',');


rawPaml = " , , ,b,y,r,...";

uniqueDefs = '';
newPixelData = [];
colorlist = [];

//pseudo code...
/*
for each def in rawPaml
    make string like this: var b='color:#000;background:#000';
    colorlist.push('b');
*/
for(var i=0;i<rDefs.length;i++){
	var def = rDefs[i].trim().split('=');
	var sym = def[0];
	var val = def[1];
	colorlist[sym] = val;
}

/*
for each line in rawPaml pixelData
    for each color in colorlist
        uniqueDefs += regexReplace(line, '('+color+'\,)+', color+',');
    add a %c before each new def symbol change
    add \n at end of line
    if doublewidth
        double each def symbol
    append modified line to newPixelData
*/

for(var i=0;i<rDPix.length;i++){
	var sym = rDPix[i].trim();
	if (sym.length == 0) {
		sym = ' ';
	}
	newPixelData.push(sym);
}

/*
finally create the js call
    console.log(newPixelData, the defs: b,g,t,b,g,b,....);
    // draw canvas...
*/
console.log('Definitions\n----------------------------');
for(def in colorlist) {
	console.log(def, '=', colorlist[def]);
}
var sPrint = ''
for(var i=0;i<newPixelData.length;i++){
	sPrint = sPrint + newPixelData[i] + ',';
}
console.log(sPrint);
