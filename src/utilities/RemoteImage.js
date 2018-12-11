
import axios from 'axios';

export class RemoteImage {
  constructor(imageUrl) {
    this._imageUrl = imageUrl;
  }

  get imageBase64() {
    return axios.get(this._imageUrl, {
      responseType: 'arraybuffer'
    })
    .then(response => {
      return `data:${response.headers['content-type']};base64,${btoa(String.fromCharCode(...new Uint8Array(response.data)))}`;
    });

  }
}