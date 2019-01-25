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
var rDPix = paml.getElementsByTagName('drawpixels')[0].textContent.trim().split('\n');


metadata = [];
PixelData = [];
colorlist = [];


/////////////////// Parse Metadata ///////////////////
for(var i=0;i<rInfo.length;i++){
	var line = rInfo[i].split(';')[0].trim(); // remove comments
	var raw = line.trim().split('=');
	var prop = raw[0].toLowerCase().trim();
	if (prop[0] == ';') // watch for comments
		continue;
	var val = raw[1];
	if (['xpixels','ypixels','sizexpixels','sizeypixels'].includes(prop) ) {
		val = parseInt(val); // read as number
	}
	metadata[prop] = val;
}


/////////////////// Parse color definitions ///////////////////
/* pseudo code...
for each def in rawPaml
    make string like this: var b='color:#000;background:#000';
    colorlist.push('b');
*/
for(var i=0;i<rDefs.length;i++){
	var line = rDefs[i].split(';')[0].trim(); // remove comments
	var def = line.trim().split('=');
	var sym = def[0].trim();
	if (sym.length == 0 || def.length < 2) // skip blank lines
		continue;
	var val = def[1];
	colorlist[sym] = val;
}
//add default transparent
colorlist[' '] = 'transparent';
if (typeof metadata['bgcolor'] == 'string' && metadata['bgcolor'].length > 1) {
	// apply bg color if specified...
	colorlist[' '] = metadata['bgcolor'];
}

/////////////////// Parse pixel table ///////////////////
// pre-process for line-comments
for(var i=0;i<rDPix.length;i++){
	var line = rDPix[i].split(';')[0].trim(); // remove comments
	rDPix[i] = line;
}
rDPix = rDPix.join('').split(',');
/* pseudo code...
for each line in rawPaml pixelData
    for each color in colorlist
        uniqueDefs += regexReplace(line, '('+color+'\,)+', color+',');
    add a %c before each new def symbol change
    add \n at end of line
    if doublewidth
        double each def symbol
    append modified line to PixelData
*/
for(var i=0;i<rDPix.length;i++){
	var sym = rDPix[i].trim();
	if (sym.length == 0) {
		sym = ' ';
	}
	PixelData.push(sym);
}
// check for erronous length and fill with transparent pixels
var ePixel = (PixelData.length % metadata['xpixels']);
if (ePixel>0) {
	if (ePixel > 2) {
		var nError = PixelData.length - ePixel;
		for(var i=0;i<nError;i++) {
			PixelData.push(' ');
		}
	} else {
		PixelData = PixelData.slice(0,-1);
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
		result += pixels[i].repeat(2); // print double for monospace font width in Chrome & Firefox
	}
	var ConsoleOutParams = [result].concat(arrBrushes);
	console.log.apply(null,ConsoleOutParams);
	// console.log(ConsoleOutParams); //debug info
}

function Matrix(w,h) { //see https://stackoverflow.com/a/41815396/883015
	return Array(h).fill(null).map(() => Array(w).fill(0));
}

/////////////////// Print parse data ///////////////////
/* pseudo code...
finally create the js call
    console.log(PixelData, the defs: b,g,t,b,g,b,....);
    // draw canvas...
*/
console.log('\nMetadata\n----------------------------');
for(prop in metadata) {
	console.log(prop, '=', metadata[prop]);
}

console.log('\nDefinitions\n----------------------------');
for(def in colorlist) {
	console.log(def, '=', colorlist[def]);
}

console.log('\nPixel table\n----------------------------');
var sPrint = ''
for(var i=0;i<PixelData.length;i++){
	if (i>0 && i % metadata['xpixels'] == 0) {
		// insert break
		sPrint += '\n';
	}
	sPrint = sPrint + PixelData[i] + ',';
}
console.log(sPrint);

console.log('\nResult\n----------------------------');
consoleDrawPAML(colorlist,PixelData,metadata); //formated console.log not fully supported on Edge or IE11



//canvas draw test
var iWidth = metadata['xpixels'] * metadata['sizexpixels'];
var iHeight = metadata['ypixels'] * metadata['sizeypixels'];

document.body.innerHTML = '<canvas id=PAMLCANVAS width='+iWidth+' height='+iHeight+' style="width:'+iWidth+'px;height:'+iHeight+'px;"></canvas>' + document.body.innerHTML;
pCnv = document.getElementById('PAMLCANVAS');
pCtx = pCnv.getContext('2d');


// turn off image smoothing / anti-aliasing
pCtx.imageSmoothingEnabled       = false;
pCtx.webkitImageSmoothingEnabled = false;
pCtx.mozImageSmoothingEnabled    = false;
pCtx.msImageSmoothingEnabled     = false;
pCtx.oImageSmoothingEnabled      = false;
no_aa_css = `canvas { /* https://stackoverflow.com/a/7665647/883015 */
	image-rendering: optimizeSpeed;             /* Older versions of FF          */
	image-rendering: -moz-crisp-edges;          /* FF 6.0+                       */
	image-rendering: -webkit-optimize-contrast; /* Safari                        */
	image-rendering: -o-crisp-edges;            /* OS X & Windows Opera (12.02+) */
	image-rendering: pixelated;                 /* Awesome future-browsers       */
	-ms-interpolation-mode: nearest-neighbor;   /* IE                            */
}`;
var sheet = window.document.styleSheets[0];
if (typeof sheet == 'undefined') { // create new blank stylesheet if n/a
	document.head.innerHTML += '<style></style>'
	sheet = window.document.styleSheets[0];
}
sheet.insertRule(no_aa_css, sheet.cssRules.length);


var img_x = 0, img_y = 0 - metadata['sizeypixels'];
var pcount, pstart = 0, pstop;
var pseparator = ',';
var ptotal = PixelData.length;
var pvalue = '';

for (pcount = 0; pcount < ptotal; ++pcount) 
{
	img_x = (pcount%metadata['xpixels'])*metadata['sizexpixels'];
	if ((pcount%metadata['xpixels'])==0)
		img_y += metadata['sizeypixels'];
	pCtx.fillStyle = colorlist[ PixelData[pcount] ];
	pCtx.fillRect(img_x,img_y,metadata['sizexpixels'],metadata['sizeypixels']);
}

