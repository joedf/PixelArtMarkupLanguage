var parser = new DOMParser();
var doc = parser.parseFromString(sxx, "application/xml");
var paml = doc.getElementsByTagName('paml')[0];

paml.getElementsByTagName('info')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
paml.getElementsByTagName('defcolor')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
paml.getElementsByTagName('drawpixels')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');


rawPaml = " , , ,b,y,r,...";

uniqueDefs = '';
newPixelData = '';
colorlist = [];

/* //pseudo code...
for each def in rawPaml
    make string like this: var b='color:#000;background:#000';
    colorlist.push('b');

for each line in rawPaml pixelData
    for each color in colorlist
        uniqueDefs += regexReplace(line, '('+color+'\,)+', color+',');
    add a %c before each new def symbol change
    add \n at end of line
    if doublewidth
        double each def symbol
    append modified line to newPixelData

finally create the js call
    console.log(newPixelData, the defs: b,g,t,b,g,b,....);
/*
