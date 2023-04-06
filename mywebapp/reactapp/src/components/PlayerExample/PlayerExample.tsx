import React,{ Component, createRef } from 'react';
import { Player, PlayerReference, StaticPlayerInstanceMethods } from 'video-react';
import { Button, Form, FormGroup, Label, Input } from 'reactstrap';

interface Props {
    // trackPlaying: TrackModel;
    // shuffle: boolean;
    // repeat: Repeat;
  }
  
  interface State {
    playerSource: string;
    inputVideoUrl: string;
  }
export class PlayerExample extends React.Component<Props,State> {
  player:React.RefObject<PlayerReference>;

  constructor(props:Props) {
    super(props);
    this.player=React.createRef();

    this.state = {
      playerSource: 'test.mp4',
      inputVideoUrl: 'test.mkv'
    };
    this.handleValueChange = this.handleValueChange.bind(this);
    this.updatePlayerInfo = this.updatePlayerInfo.bind(this);
  }

  componentDidUpdate(prevProps:any, prevState:any) {
    if (this.state.playerSource !== prevState.playerSource) {
        if(this.player.current){
            this.player.current?.load();
        }
    }
  }

  handleValueChange(e:React.ChangeEvent<HTMLInputElement>) {
    const { value } = e.target;
    this.setState({
      inputVideoUrl: value
    });
  }

  updatePlayerInfo() {
    const { inputVideoUrl } = this.state;
    this.setState({
      playerSource: inputVideoUrl
    });
  }

  render() {
    return (
      <div>
      <Player ref={this.player} >
          <source src={this.state.playerSource} />
        </Player>
        <div className="docs-example">
          <Form>
            <FormGroup>
              <Label for="inputVideoUrl">Video Url</Label>
              <Input
                name="inputVideoUrl"
                id="inputVideoUrl"
                value={this.state.inputVideoUrl}
                onChange={this.handleValueChange}
              />
            </FormGroup>
            <FormGroup>
              <Button type="button" onClick={this.updatePlayerInfo}>
                Update
              </Button>
            </FormGroup>
          </Form>
        </div>
      </div>
    );
  }
}

export  class Form1 extends React.Component<Props>{
  inputRef: React.RefObject<HTMLInputElement>;

  constructor(props:Props){
    super(props);

    this.inputRef = React.createRef();
    this.test=this.test.bind(this);
  }
  test(){
      this.inputRef.current?.focus();
  }
  render() {
    return (
      <>
        <input ref={this.inputRef} />
        <button onClick={this.test}>
          Focus the input
        </button>
      </>
    );
  }
}

