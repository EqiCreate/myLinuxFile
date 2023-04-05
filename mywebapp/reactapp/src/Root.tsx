import React from 'react';
interface State {
    hasError: boolean;
  }
  type Props = {
    children?: React.ReactNode
  };
  class Root extends React.Component<Props, State> {
    constructor(props:Props){
        super(props);
        this.state={hasError:false};
    }
    componentDidCatch(error: Error, errorInfo: React.ErrorInfo): void {
        console.error(`mm crashed: ${error}`);
        this.setState({ hasError: true });
    }
    render() {
        if(this.state.hasError){
               return (
                <div>
                    <p>
                    <span>{' '} 
                        wrong</span>
                    </p>
                </div>
                
               );     
        }
        return this.props.children;
    }
}

export default Root;