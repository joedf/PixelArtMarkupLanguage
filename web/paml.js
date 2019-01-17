var parser = new DOMParser();
var doc = parser.parseFromString(sxx, "application/xml");
var paml = doc.getElementsByTagName('paml')[0];

paml.getElementsByTagName('info')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
paml.getElementsByTagName('defcolor')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
paml.getElementsByTagName('drawpixels')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
