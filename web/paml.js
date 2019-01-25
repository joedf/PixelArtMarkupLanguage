/* test */

var rawPaml = `<paml>

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
var doc = parser.parseFromString(rawPaml, "application/xml");
var paml = doc.getElementsByTagName('paml')[0];

var rInfo = paml.getElementsByTagName('info')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
var rDefs = paml.getElementsByTagName('defcolor')[0].textContent.trim().split('\n');
var rDPix = paml.getElementsByTagName('drawpixels')[0].textContent.trim().split(',');

uniqueDefs = '';
metadata = [];
newPixelData = [];
colorlist = [];


/////////////////// Parse Metadata ///////////////////
for(var i=0;i<rInfo.length;i++){
	var raw = rInfo[i].trim().split('=');
	var prop = raw[0];
	var val = raw[1].split(';')[0].trim(); // remove comments 
	metadata[prop] = val;
}


/////////////////// Parse color definitions ///////////////////
/* pseudo code...
for each def in rawPaml
    make string like this: var b='color:#000;background:#000';
    colorlist.push('b');
*/
for(var i=0;i<rDefs.length;i++){
	var def = rDefs[i].trim().split('=');
	var sym = def[0];
	var val = def[1].split(';')[0].trim(); // remove comments 
	colorlist[sym] = val;
}
//add default transparent
colorlist[' '] = 'transparent';

/////////////////// Parse pixel table ///////////////////
/* pseudo code...
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
// check for erronous length and fill with transparent pixels
var ePixel = (newPixelData.length % metadata['xpixels']);
if (ePixel>0) {
	if (ePixel > 2) {
		var nError = newPixelData.length - ePixel;
		for(var i=0;i<nError;i++) {
			newPixelData.push(' ');
		}
	} else {
		newPixelData = newPixelData.slice(0,-1);
	}
}


/////////////////// Define functions ///////////////////
function color2css(colorcode) {
	if (colorcode == ' ') //color2css(' '); // defaults to transparent...
		colorcode = 'transparent';
	return 'color:'+colorcode+';background:'+colorcode;
}

function consoleDrawPAML(colors,pixels,metadata) {
	var result = '';
	var brush = '\0';
	var arrBrushes = [];
	for(var i=0;i<pixels.length;i++){
		if (brush != pixels[i]) { // color change
			brush = pixels[i];
			result += '%c';
			arrBrushes.push( color2css(colors[brush]) );
		}
		if (i>0 && i % metadata['xpixels'] == 0) {
			// insert break
			result += '\n';
		}
		result += pixels[i]+pixels[i]; // print double for monospace font width in Chrome & Firefox
	}
	var ConsoleOutParams = [result].concat(arrBrushes);
	console.log.apply(null,ConsoleOutParams);
	// console.log(ConsoleOutParams); //debug info
}

/////////////////// Print parse data ///////////////////
/* pseudo code...
finally create the js call
    console.log(newPixelData, the defs: b,g,t,b,g,b,....);
    // draw canvas...
*/
console.log('Metadata\n----------------------------');
for(prop in metadata) {
	console.log(prop, '=', metadata[prop]);
}
console.log('Definitions\n----------------------------');
for(def in colorlist) {
	console.log(def, '=', colorlist[def]);
}
console.log('Pixel table\n----------------------------');
var sPrint = ''
for(var i=0;i<newPixelData.length;i++){
	if (i>0 && i % metadata['xpixels'] == 0) {
		// insert break
		sPrint += '\n';
	}
	sPrint = sPrint + newPixelData[i] + ',';
}
console.log(sPrint);
consoleDrawPAML(colorlist,newPixelData,metadata);
