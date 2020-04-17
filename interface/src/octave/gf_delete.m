% FUNCTION [...] = gf_delete([operation [, args]])
%
%   Delete an existing getfem object from memory (mesh, mesh_fem, etc.).
%   
%   SEE ALSO:
%   gf_workspace, gf_mesh, gf_mesh_fem.
%   
%
%   * gf_delete(I[, J, K,...])
%   
%   I should be a descriptor given by gf_mesh(),
%   gf_mesh_im(), gf_slice() etc.
%   
%   Note that if another object uses I, then object I will be deleted only
%   when both have been asked for deletion.
%   
%   Only objects listed in the output of gf_workspace('stats') can be
%   deleted (for example gf_fem objects cannot be destroyed).
%   
%   You may also use gf_workspace('clear all') to erase everything at
%   once.
%   
%
%
function [varargout]=gf_delete(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_octave('delete', varargin{:});
  else
    gf_octave('delete', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
