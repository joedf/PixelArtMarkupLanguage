PAML Specifications
--------------------------------------

PAML (Pixel Art Markup Language) is a pseudo-XML compliant form of structured data. The are some variations from traditional XML that are non-breaking allowing PAML to be parsed by modern XML parsers. This means PAML is still subject to some XML rules, such as correctly using the ampersand

## Tags
There are 4 required tags:
- `<paml>`
- `<info>`
- `<defcolor>`
- `<drawpixels>`

The `<paml>` tag is used to enclose the all other tags and their associated information. It delimits when the PAML file start and when it ends (`</paml>`).
  
The `<info>` tag is used to define metadata, options and the required information to correctly draw out the pixels later on. The properties
author=Nintendo
				dateofcreation=2013-05-31
				license=Recreated_by_Joe_DF
				xpixels=19
				ypixels=20
				sizexpixels=10
				sizeypixels=10
				bgcolor=#0055AA ;blue