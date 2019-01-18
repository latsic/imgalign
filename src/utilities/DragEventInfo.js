
export class DragEventInfo {
  constructor(dragEvent) {
    
    if(!dragEvent || !dragEvent.dataTransfer) {
      throw new Error('Invalid drag event');
    }
    this.dragEvent = dragEvent;
  }

  get mayContainImageData() {
    return this.dragEvent.dataTransfer.types.some(value => {
      return value == 'Files'
          || value == 'text/html'
          || value == 'URL'
          || value == 'text/plain';
    })
  }

  get containsImageData() {
    return this.imageFile || this.imageUrlBase64 || this.imageTextBase64;
  }

  get imageFile() {
    if( !this.dragEvent.dataTransfer.files
      || this.dragEvent.dataTransfer.files.length == 0) {
      return null;
    }
    const file = this.dragEvent.dataTransfer.files[0];
    return file.type == 'image/png' || file.type == 'image/jpeg'
      ? file
      : null;
  }

  get imageUrlRemote() {
    const dataText =
         this.dragEvent.dataTransfer.getData('text/html')
      || this.dragEvent.dataTransfer.getData('URL');
    if(!dataText) return null;

    const regEx = /<img.+src=(?:"|')(.+?(\.jpg|\.png))(?:"|')(?:.+?)>/;
    const match = regEx.exec(dataText);
    return match && match.length == 3
      ? match[1]
      : null;
  }

  get imageUrlBase64() {

    const dataText =
         this.dragEvent.dataTransfer.getData('text/html')
      || this.dragEvent.dataTransfer.getData('URL');
    if(!dataText) return null;

    const regEx = /src="(data:image\/([a-zA-Z]*);base64,([^"]*))"/;
    const match = regEx.exec(dataText);
    return match && match.length == 4
      ? match[1]
      : null;
  }
  get imageTextBase64() {

    const dataText = this.dragEvent.dataTransfer.getData('text/plain')
    if(!dataText) return null;

    const regEx = /(data:image\/([a-zA-Z]*);base64,([^"]*))/;
    const match = regEx.exec(dataText);
    return match && match.length == 4
      ? match[1]
      : null;
  }

  get fieldOfView() {

    let val = this.dragEvent.dataTransfer.getData('number/fieldOfView');
    if(val) val = Number(val);
    return val && !isNaN(val)
      ? val
      : null;
  }

  get projected() {

    let val = this.dragEvent.dataTransfer.getData('number/projected');
    if(val) val = Number(val);
    return val && !isNaN(val)
      ? val
      : null;
  }
}