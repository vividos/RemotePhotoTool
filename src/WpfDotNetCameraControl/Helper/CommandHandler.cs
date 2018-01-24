using System;
using System.Windows.Input;

namespace WpfDotNetCameraControl.Helper
{
    /// <summary>
    /// Command handler implementation that calls an action
    /// </summary>
    public class CommandHandler : ICommand
    {
        /// <summary>
        /// Action to execute
        /// </summary>
        private readonly Action executeAction;

        /// <summary>
        /// Event that gets signaled when the "can execute" state has changed
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add
            {
                // not used
            }

            remove
            {
                // not used
            }
        }

        /// <summary>
        /// Creates new command handler
        /// </summary>
        /// <param name="executeAction">action to execute when command is executed</param>
        public CommandHandler(Action executeAction)
        {
            this.executeAction = executeAction;
        }

        /// <summary>
        /// Called to check if command can be executed
        /// </summary>
        /// <param name="parameter">parameter; not used</param>
        /// <returns>true always</returns>
        public bool CanExecute(object parameter)
        {
            return true;
        }

        /// <summary>
        /// Executes command
        /// </summary>
        /// <param name="parameter">parameter; not used</param>
        public void Execute(object parameter)
        {
            this.executeAction();
        }
    }
}
