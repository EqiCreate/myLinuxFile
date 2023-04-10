import React,{ Component, createRef } from 'react';
import { Player, PlayerReference, StaticPlayerInstanceMethods } from 'video-react';
import { Button, Form, FormGroup, Label, Input } from 'reactstrap';
import { connect, useSelector } from 'react-redux';
import { RootState } from '../../store/reducers';
import { TrackModel } from '../../shared/types/BLL';

// import fs from 'fs';
// interface Props {
//     // trackPlaying: TrackModel;
//     // shuffle: boolean;
//     // repeat: Repeat;
//   }

interface InjectProp {
  playerSource: TrackModel[];
  // inputVideoUrl?: string;
}
  type Props = InjectProp;
  
 
export class PlayerExample extends React.Component<Props> {
  player:React.RefObject<PlayerReference>;
  // const Studio = useSelector((state: RootState) => state.studio);
  
  constructor(props:Props) {
    super(props);
    this.player=React.createRef();
    console.log("constructor for player");

    // this.state = {
    //   playerSource: 'test.mp4',
    //   inputVideoUrl: 'test.mkv'
    // };
    // this.handleValueChange = this.handleValueChange.bind(this);
    // this.updatePlayerInfo = this.updatePlayerInfo.bind(this);
  }
  componentDidMount(): void {
    console.log("componentDidMount for player");
  }
  componentDidUpdate(prevProps:any, prevState:any) {
    console.log("componentDidUpdate for player");

    // const { playerSource } = this.props;

    // if ( playerSource !== prevProps) {
        if(this.player.current){
            this.player.current?.load();
            // this.player.current?.pause();
            this.player.current?.play();
        }
    // }
  }
  componentWillUnmount(): void {
    console.log("componentWillUnmount for player");
    
  }

  // handleValueChange(e:React.ChangeEvent<HTMLInputElement>) {
  //   const { value } = e.target;
  //   this.setState({
  //     inputVideoUrl: value
  //   });
  // }

  updatePlayerInfo() {
    // const { inputVideoUrl } = this.props;
    // this.setState({
    //   playerSource: inputVideoUrl
    // });
  }

  openDialog(){
      // fs.readdir("~/",(err,files)=>{
      //   if(err){
      //     throw err;
      //   }
      //   console.log(files);
      // })
  }

  render() {
    const { playerSource} = this.props;
    if(playerSource.length==0){
        return <div></div>
    }
    return (
      <div>
      <Player ref={this.player} >
          <source src={playerSource[playerSource?.length-1].path} />
        </Player>
        <div className="docs-example">
          <Form>
            <FormGroup>
              {/* <Label for="inputVideoUrl">Video Url</Label> */}
              {/* <Input
                name="inputVideoUrl"
                id="inputVideoUrl"
                value={inputVideoUrl}
                onChange={this.handleValueChange}
              /> */}
            </FormGroup>
            <FormGroup>
              {/* <Button type="button" onClick={this.updatePlayerInfo}>
                Update
              </Button> */}
              {/* <Button type='button' onClick={this.openDialog}></Button> */}
            </FormGroup>
          </Form>
        </div>
      </div>
    );
  }
}

// export  class Form1 extends React.Component<Props>{
//   inputRef: React.RefObject<HTMLInputElement>;

//   constructor(props:Props){
//     super(props);

//     this.inputRef = React.createRef();
//     this.test=this.test.bind(this);
//   }
//   test(){
//       this.inputRef.current?.focus();
//   }
//   render() {
//     return (
//       <>
//         <input ref={this.inputRef} />
//         <button onClick={this.test}>
//           Focus the input
//         </button>
//       </>
//     );
//   }
// }

const mapStateToProps = (root: RootState):InjectProp=> ({
  // playerSource:"",
  playerSource:root.studio.queue
});

export default connect(mapStateToProps)(PlayerExample);


