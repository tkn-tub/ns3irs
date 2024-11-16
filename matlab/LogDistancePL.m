classdef (Sealed,StrictDefaults) LogDistancePL < phased.internal.AbstractFreeSpace

    properties (Nontunable) 
        Exponent = 3.0
        ReferenceDistance = 1.0
        ReferenceLoss = 46.6777
    end
    
    properties (Access = private, Nontunable)
        %Fractional delay filter
        cFractionalDelayFilter
    end
    
    methods
        function obj = LogDistancePL(varargin)
            obj@phased.internal.AbstractFreeSpace(varargin{:});
        end
    end

    methods (Access = protected)

        function setupImpl(obj,varargin)
            setupImpl@phased.internal.AbstractFreeSpace(obj,varargin{:})
            obj.pLambda = obj.PropagationSpeed/obj.OperatingFrequency;
            obj.cFractionalDelayFilter = dsp.VariableFractionalDelay;
        end
        
        function releaseImpl(obj)
            releaseImpl@phased.internal.AbstractFreeSpace(obj);
            release(obj.cFractionalDelayFilter);
        end

        function resetImpl(obj)
            resetImpl@phased.internal.AbstractFreeSpace(obj);
            reset(obj.cFractionalDelayFilter);
        end

        function s = saveObjectImpl(obj)
            s = saveObjectImpl@phased.internal.AbstractFreeSpace(obj);
            if isLocked(obj)
                s.cFractionalDelayFilter = saveobj(obj.cFractionalDelayFilter);
            end
        end

        function s = loadSubObjects(obj,s,wasLocked)
            s = loadSubObjects@phased.internal.AbstractFreeSpace(obj,s,wasLocked);
            if wasLocked 
                if isfield(s,'cFractionalDelayFilter')
                    obj.cFractionalDelayFilter = ...
                        dsp.VariableFractionalDelay.loadobj(s.cFractionalDelayFilter);
                    s = rmfield(s,'cFractionalDelayFilter');
                else
                    obj.cFractionalDelayFilter = dsp.VariableFractionalDelay;
                end
            end
        end

        function loadObjectImpl(obj,s,wasLocked)
            s = loadSubObjects(obj,s,wasLocked);
            fn = fieldnames(s);
            for m = 1:numel(fn)
                obj.(fn{m}) = s.(fn{m});
            end
        end

        function [xbuf_in,ndelay] = computeDelayedSignal(obj,x,delay)
            % For between sample delays, compute nDelay and frac
            % delay the signal
            intd = fix(delay);
            fracd = delay-intd; % in samples
            % flush state, at most 1 sample
            ndelay = intd;
            temp = step(obj.cFractionalDelayFilter,complex(x),fracd);
            xbuf_in = temp(:,1:size(x,2));
        end
        
        function [y,propdelay] = computeMultiplePropagatedSignal(obj,x,...
                ncol_per_path,numOfPropPaths,startLoc,endLoc,baseVel,targetVel,Fs)
            
            y = complex(zeros(size(x)));
            k = obj.pRangeFactor;
            lambda = obj.pLambda;
            
            % Get propagation delays and distances
            [propdelay,propdistance,rspeed] = computePropagationDelayVelocity(...
                obj,startLoc,endLoc,baseVel,targetVel);
            
            % Calculate log-distance path loss for each path
            if propdistance <= obj.ReferenceDistance
                logd_loss = -obj.ReferenceLoss;
            else
                pathLoss = 10 * obj.Exponent *log10(propdistance/obj.ReferenceDistance);
                logd_loss = -obj.ReferenceLoss - pathLoss;
            end
            
            % Convert path loss to linear scale
            plossfactor = sqrt(db2pow(-logd_loss));
            
            for pIdx = 1:numOfPropPaths
                colidx = (pIdx-1)*ncol_per_path+(1:ncol_per_path);
                y(:,colidx) = exp(-1i*2*pi*k*propdistance(pIdx)/lambda)/plossfactor(pIdx)*...
                        bsxfun(@times,x(:,colidx),...
                               exp(1i*2*pi*k*rspeed(pIdx)/lambda*(propdelay(pIdx)+(0:size(x,1)-1)'/Fs)));                    
            end
        end


        function setRangeFactor(obj)
            obj.pRangeFactor = 1;
        end
        
        function validateNumberOfPositionPairs(obj,x,startLocSize,endLocSize) %#ok<INUSL>
            coder.extrinsic('mat2str');
            coder.extrinsic('num2str');            

            numPosPairs = size(x,2);
            
            if numPosPairs == 1
                expDim = '[3 ';
            else
                expDim = '[3 1] or [3 ';
            end
            cond =  ~(isequal(startLocSize,[3 numPosPairs]) || isequal(startLocSize,[3 1]));
            if cond
                coder.internal.errorIf(cond, ...
                     'MATLAB:system:invalidInputDimensions','Pos1',...
                        [expDim num2str(numPosPairs) ']'], ...
                               mat2str(startLocSize));
            end


            cond =  ~(isequal(endLocSize,[3 numPosPairs]) || isequal(endLocSize,[3 1]));
            if cond
                coder.internal.errorIf(cond, ...
                     'MATLAB:system:invalidInputDimensions','Pos2',...
                        [expDim num2str(numPosPairs) ']'], ...
                               mat2str(endLocSize));
            end
            
            cond =   ~(isequal(startLocSize,[3 numPosPairs]) || ...
                      isequal(endLocSize,[3 numPosPairs]));
            if cond
                coder.internal.errorIf(cond, ...
                     'phased:phased:FreeSpace:AtLeastOneNotColumnVect','Pos1','Pos2',numPosPairs);
            end
            
            cond = ~ (isequal(startLocSize,[3 1]) || isequal(endLocSize,[3 1]));
            if cond
                coder.internal.errorIf(cond, ...
                     'phased:phased:FreeSpace:AtLeastOneColumnVect','Pos1','Pos2');
            end
            
        end
    end
    
    methods (Access = protected, Static, Hidden)
        function header = getHeaderImpl
          header = matlab.system.display.Header(...
              'Title',getString(message('phased:library:block:FreeSpaceTitle')),...
              'Text',getString(message('phased:library:block:FreeSpaceDesc')));
        end
        function groups = getPropertyGroupsImpl
            groups = matlab.system.display.Section(...
                'phased.FreeSpace');
            groups.PropertyList = groups.PropertyList([2:3 1 4:end]);
            dMaximumDistanceSource = matlab.system.display.internal.Property(...
                'MaximumDistanceSource','IsGraphical',false,...
                'UseClassDefault',false,'Default','Property');
            dMaximumNumInputSamplesSource = matlab.system.display.internal.Property(...
                'MaximumNumInputSamplesSource','IsGraphical',false);
            dMaximumNumInputSamples = matlab.system.display.internal.Property(...
                'MaximumNumInputSamples','IsGraphical',false);
            % dSampleRate = matlab.system.display.internal.Property(...
            %     'SampleRate','IsObjectDisplayOnly',true);
            for m = 1:numel(groups.PropertyList)
                if strcmp(groups.PropertyList{m},'MaximumDistanceSource')
                    groups.PropertyList{m} = dMaximumDistanceSource;
                % elseif strcmp(groups.PropertyList{m},'SampleRate')
                %     groups.PropertyList{m} = dSampleRate;
                elseif strcmp(groups.PropertyList{m},'MaximumNumInputSamplesSource')
                    groups.PropertyList{m} = dMaximumNumInputSamplesSource;
                elseif strcmp(groups.PropertyList{m},'MaximumNumInputSamples')
                    groups.PropertyList{m} = dMaximumNumInputSamples;
                end
            end
        end
    end
    
    methods (Access = protected)
                
        function str = getIconImpl(obj) %#ok<MANU>
            str = sprintf('Free Space\nChannel');
        end
    end
    
end
