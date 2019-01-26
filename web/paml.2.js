
// MIT License - Joe DF 2018
class PAML {
	constructor(rawPaml) {
		this.rawPaml = rawPaml;

		this.Metadata = [];
		this.colorlist = [];
		this.PixelData = [];
	}

	parseXML() {
		this.pamlXML = this.preprocess(this.rawPaml);
		try {
			var parser = new DOMParser();
			var doc = parser.parseFromString(this.pamlXML, "application/xml");
			this.paml = doc.getElementsByTagName('paml')[0];
		} catch (e) {
			// retry with '&' changed...
			this.pamlXML = this.pamlXML.replace(/&/g,'and');

			var parser = new DOMParser();
			var doc = parser.parseFromString(this.pamlXML, "application/xml");
			this.paml = doc.getElementsByTagName('paml')[0];
		}
		return this.paml;
	}

	preprocess(original) {
		var processedPAML = '';
		var lines = original.split('\n');
		for(var i=0;i<lines.length;i++) {
			// purge comments
			processedPAML += lines[i].split(';')[0] + '\n';
		}
		return processedPAML;
	}

	parse() {
		this.parseXML();

		var rInfo = this.paml.getElementsByTagName('info')[0].textContent.replace(/[ \t]/ig,'').trim().split('\n');
		var rDefs = this.paml.getElementsByTagName('defcolor')[0].textContent.trim().split('\n');
		var rDPix = this.paml.getElementsByTagName('drawpixels')[0].textContent.trim().split('\n');

		// reset vars
		this.Metadata = [];
		this.colorlist = [];
		this.PixelData = [];

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
			this.Metadata[prop] = val;
		}

		// default these to 1 if unspecified
		if (typeof this.Metadata['sizexpixels'] != 'number') {
			this.Metadata['sizexpixels'] = 1;
		}
		if (typeof this.Metadata['sizeypixels'] != 'number') {
			this.Metadata['sizeypixels'] = 1;
		}

		/////////////////// Parse color definitions ///////////////////
		for(var i=0;i<rDefs.length;i++){
			var line = rDefs[i].split(';')[0].trim(); // remove comments
			var def = line.trim().split('=');
			var sym = def[0].trim();
			if (sym.length == 0 || def.length < 2) // skip blank lines
				continue;
			var val = def[1];
			this.colorlist[sym] = val;
		}
		//add default transparent
		this.colorlist[' '] = 'transparent';
		if (typeof this.Metadata['bgcolor'] == 'string' && this.Metadata['bgcolor'].length > 1) {
			// apply bg color if specified...
			this.colorlist[' '] = this.Metadata['bgcolor'];
		}

		/////////////////// Parse pixel table ///////////////////
		// pre-process for line-comments
		for(var i=0;i<rDPix.length;i++){
			var line = rDPix[i].split(';')[0].trim(); // remove comments
			rDPix[i] = line;
		}
		rDPix = rDPix.join('').split(',');
		for(var i=0;i<rDPix.length;i++){
			var sym = rDPix[i].trim();
			if (sym.length == 0) {
				sym = ' ';
			}
			this.PixelData.push(sym);
		}
		// check for erronous length and fill with transparent pixels
		var ePixel = (this.PixelData.length % this.Metadata['xpixels']);
		if (ePixel>0) {
			if (ePixel > 2) {
				var nError = this.PixelData.length - ePixel;
				for(var i=0;i<nError;i++) {
					this.PixelData.push(' ');
				}
			} else {
				this.PixelData = this.PixelData.slice(0,-1);
			}
		}
		return this;
	}

	color2css(colorcode) {
		if (colorcode == ' ') //color2css(' '); // defaults to transparent...
			colorcode = 'transparent';
		return 'color:'+colorcode+';background:'+colorcode;
	}

	consoleDraw() {
		var result = '';
		var brush = '\0';
		var arrBrushes = [];
		for(var i=0;i<this.PixelData.length;i++){
			if (brush != this.PixelData[i]) { // color change
				brush = this.PixelData[i];
				result += '%c';
				arrBrushes.push( this.color2css(this.colorlist[brush]) );
			}
			if (i>0 && i % this.Metadata['xpixels'] == 0) {
				// insert break
				result += '\n';
			}
			result += this.PixelData[i].repeat(2); // print double for monospace font width in Chrome & Firefox
		}
		var ConsoleOutParams = [result].concat(arrBrushes);
		// formated console.log not fully supported on Edge or IE11
		console.log.apply(null,ConsoleOutParams);
		// console.log(ConsoleOutParams); //debug info
	}

	consolePrintDebug() {
		console.log('\nMetadata\n----------------------------');
		for(var prop in this.Metadata) {
			console.log(prop, '=', this.Metadata[prop]);
		}

		console.log('\nDefinitions\n----------------------------');
		for(var def in this.colorlist) {
			console.log(def, '=', this.colorlist[def]);
		}

		console.log('\nPixel table\n----------------------------');
		var sPrint = ''
		for(var i=0;i<this.PixelData.length;i++){
			if (i>0 && i % this.Metadata['xpixels'] == 0) {
				// insert break
				sPrint += '\n';
			}
			sPrint = sPrint + this.PixelData[i] + ',';
		}
		console.log(sPrint);

		console.log('\nResult\n----------------------------');
		this.consoleDraw();
	}

	canvasDraw(htmlElement) {
		var iWidth = this.Metadata['xpixels'] * this.Metadata['sizexpixels'];
		var iHeight = this.Metadata['ypixels'] * this.Metadata['sizeypixels'];

		htmlElement.style.width = ( iWidth )+'px';
		htmlElement.style.height = ( iHeight )+'px';
		htmlElement.setAttribute('width',iWidth);
		htmlElement.setAttribute('height',iHeight);

		var pCnv = htmlElement;
		var pCtx = pCnv.getContext('2d');

		// turn off image smoothing / anti-aliasing
		pCtx.imageSmoothingEnabled       = false;
		pCtx.webkitImageSmoothingEnabled = false;
		pCtx.mozImageSmoothingEnabled    = false;
		pCtx.msImageSmoothingEnabled     = false;
		pCtx.oImageSmoothingEnabled      = false;
		htmlElement.style.cssText += "image-rendering:optimizeSpeed; \
						image-rendering: optimizeSpeed; \
						image-rendering: -moz-crisp-edges; \
						image-rendering: -webkit-optimize-contrast; \
						image-rendering: -o-crisp-edges; \
						image-rendering: pixelated; \
						-ms-interpolation-mode: nearest-neighbor;";

		var img_x = 0, img_y = 0 - this.Metadata['sizeypixels'];

		for (var i = 0; i < this.PixelData.length; ++i) {
			img_x = (i%this.Metadata['xpixels'])*this.Metadata['sizexpixels'];
			if ((i%this.Metadata['xpixels'])==0)
				img_y += this.Metadata['sizeypixels'];
			pCtx.fillStyle = this.colorlist[ this.PixelData[i] ];
			pCtx.fillRect(img_x,img_y,this.Metadata['sizexpixels'],this.Metadata['sizeypixels']);
		}
	}
}
